#pragma once
#ifndef TRDROP_TASKS_PRE_TEAR_H
#define TRDROP_TASKS_PRE_TEAR_H

#include <functional>
#include <algorithm>
#include <math.h>

#include "Tasks.h"
#include "Either.h"

#include "teardata.h"


namespace trdrop {
	namespace tasks {
		namespace pre {

			using trdrop::tasks::pretask;
			using trdrop::Either;
			using trdrop::Right;
			using trdrop::Left;

			class TearPreTask : public pretask {

				// types
			private:
				using EitherSVD = Either<std::string, trdrop::tear_data>;
				using RightVD = Right<trdrop::tear_data>;
				using LeftS = Left<std::string>;

				// default member
			public:
				TearPreTask() = delete;
				TearPreTask(const TearPreTask & other) = default;
				TearPreTask & operator=(const TearPreTask & other) = delete;
				TearPreTask(TearPreTask && other) = default;
				TearPreTask & operator=(TearPreTask && other) = delete;
				~TearPreTask() = default;

				// specialized member
			public:
				TearPreTask(std::string id, size_t videoCount, int pixelTolerance, double lineTolerance, int windowSize)
					: id(id)
					, tearTaskData(videoCount)
					, tear_unprocessed(videoCount)
					, pixelTolerance(pixelTolerance)
					, lineTolerance(lineTolerance)
					, windowSize(windowSize)
					, videoCount(videoCount)
					, pretask(std::bind(&TearPreTask::process
						, this
						, std::placeholders::_1
						, std::placeholders::_2
						, std::placeholders::_3
						, std::placeholders::_4))
				{ 
					// zero fps-values for every video
					util::enumerate(tear_unprocessed.begin(), tear_unprocessed.end(), 0, [&](size_t ix, std::vector<double> &v) {
						v.insert(v.end(), windowSize, 0.0);
					});
				}

				// interface methods
			public:
				void process(const cv::Mat & prev, const cv::Mat & cur, const size_t currentFrameIndex, const size_t vix) {
					
					cv::Mat diffMat;
					cv::absdiff(prev, cur, diffMat);
					
					std::vector<std::vector<int>> blankLines(videoCount, std::vector<int>(diffMat.rows, 0));

					for (int i = 0; i < diffMat.rows; ++i)
					{
						// starts with a blank pixel
						if (diffMat.at<uchar>(i, 0) <= pixelTolerance)
						{
							int j = 0;
							for (; j < diffMat.cols; ++j)
							{
								if (diffMat.at<uchar>(i, j) > pixelTolerance) break;
							}
							if (j == diffMat.cols) blankLines[vix][i] = 1;
						}
					}

					static std::mutex mutex;
					std::lock_guard<std::mutex> lock(mutex);

					int maxTear = maximumContOccurence(blankLines[vix]);
					int lineToleranceCount = (int)(lineTolerance * prev.size().height);
					if (maxTear < lineToleranceCount) maxTear = 0;
					
					// count the bad pixels and if they consume more than 50% of the image -> tear (shown with a 1)
					tearTaskData.tears[vix] = static_cast<double>(maxTear) / static_cast<double>(diffMat.rows);

					size_t localIndex = currentFrameIndex % windowSize;
					tear_unprocessed[vix][localIndex] = static_cast<double>(maxTear) / static_cast<double>(diffMat.rows);
					tearTaskData.tear_unprocessed = tear_unprocessed;
					result = EitherSVD(RightVD(tearTaskData));

#if _TR_DEBUG
					std::cout << "DEBUG: TearPreTask - finished for video " << vix << '\n';
#endif
				}

				// public member
			public:
				EitherSVD result;
				const std::string id;

				// private member
			private:
				size_t videoCount;
				int pixelTolerance;
				double lineTolerance;
				int windowSize;
				trdrop::tear_data tearTaskData;
				std::vector<std::vector<double>> tear_unprocessed;

				std::function<int(std::vector<int> &)> maximumContOccurence = [&](std::vector<int> & v)
				{
					int max = 0;
					int tempMax = 0;

					for (int i = 0; i < v.size(); ++i) {
						if (v[i] == 1) {
							++tempMax;
							if (max < tempMax) max = tempMax;
						}
						else {
							tempMax = 0;
						}
					}
					return max;
				};

			};
		} // namespace pre
	} // namespace tasks
} // namespace trdrop

#endif // !TDROP_TASKS_PRE_TEAR_H
