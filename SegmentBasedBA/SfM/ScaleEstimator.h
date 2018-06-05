/**
* This file is part of SegmentBA.
*
* Copyright (C) 2017 Zhejiang University
* For more information see <https://github.com/ZJUCVG/SegmentBA>
* If you use this code, please cite the corresponding publications as 
* listed on the above website.
*
* SegmentBA is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* SegmentBA is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with SegmentBA. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _SCALE_ESTIMATOR_H_
#define _SCALE_ESTIMATOR_H_

#include "Estimator.h"
#include "ScaleEstimatorData.h"

class ScaleSolver {};
class ScaleOptimizer {};

typedef ScaleEstimatorData	SEData;
typedef float				SEMinimalSample;
typedef std::vector<float>	SENonMinimalSample;
typedef float				SEModel;
typedef ScaleSolver			SESolver;
typedef ScaleOptimizer		SEOptimizer;
typedef uint				SEIndex;

class ScaleEstimator : public Estimator<SEData, SEMinimalSample, SENonMinimalSample, SEModel, SESolver, SEOptimizer, SEIndex>
{

public:

	ScaleEstimator(const float &errTh = 0) : Estimator<SEData, SEMinimalSample, SENonMinimalSample, SEModel, SESolver, SEOptimizer, SEIndex>(errTh) {}

	virtual const uint MinimalSampleSize() const
	{
		return 1;
	}

	virtual float epsilon_exp_m(const float &epsilon) const
	{
		return epsilon;
	}

	virtual void DrawMinimalSample(const SEData &data, SEMinimalSample &sample) const
	{
		const uint i = Random::Generate(data.Size());
		sample = data.s(i);
	}

	virtual void DrawMinimalSampleOrdered(const SEData &data, const std::vector<uint> &orders, const uint &n, const bool &sampleLastOne, SEMinimalSample &sample) const
	{
		const uint i = sampleLastOne ? orders[n - 1] : orders[Random::Generate(n)];
		sample = data.s(i);
	}

	virtual void GenerateModels(SEMinimalSample &sample, AlignedVector<SEModel> &models)
	{
		models.Resize(1);
		models[0] = sample;
	}

	virtual void GenerateModels(SENonMinimalSample &sample, AlignedVector<SEModel> &models)
	{
		if(sample.empty())
			models.Resize(0);
		else
		{
			const uint ith = (sample.size() >> 1);
			std::nth_element(sample.begin(), sample.begin() + ith, sample.end());
			models.Resize(1);
			models[0] = sample[ith];
		}
	}

	virtual void VerifyModel(const SEData &data, const SEModel &model, std::vector<uint> &inliers, double &fitErr)
	{
		inliers.resize(0);
		fitErr = 0;

		 ENFT_SSE::__m128 e;
		const  ENFT_SSE::__m128 s =  ENFT_SSE::_mm_set1_ps(model);
		const  ENFT_SSE::__m128 *d1 = ( ENFT_SSE::__m128 *) data.d1s().Data(), *d2 = ( ENFT_SSE::__m128 *) data.d2s().Data();
		const uint N = data.Size(), _N = N - (N & 3);
		for(uint i = 0; i < _N; i += 4, ++d1, ++d2)
		{
			e =  ENFT_SSE::_mm_sub_ps( ENFT_SSE::_mm_mul_ps(*d1, s), *d2);
			if((e.m128_f32[0] = fabs(e.m128_f32[0])) < m_ransacErrorThreshold)
			{
				inliers.push_back(i);
				fitErr += e.m128_f32[0];
			}
			if((e.m128_f32[1] = fabs(e.m128_f32[1])) < m_ransacErrorThreshold)
			{
				inliers.push_back(i + 1);
				fitErr += e.m128_f32[1];
			}
			if((e.m128_f32[2] = fabs(e.m128_f32[2])) < m_ransacErrorThreshold)
			{
				inliers.push_back(i + 2);
				fitErr += e.m128_f32[2];
			}
			if((e.m128_f32[3] = fabs(e.m128_f32[3])) < m_ransacErrorThreshold)
			{
				inliers.push_back(i + 3);
				fitErr += e.m128_f32[3];
			}
		}
		for(uint i = _N; i < N; ++i)
		{
			e.m128_f32[0] = data.d2(i) - data.d1(i) * model;
			if(fabs(e.m128_f32[0]) < m_ransacErrorThreshold)
			{
				inliers.push_back(i);
				fitErr += e.m128_f32[0];
			}
		}
	}

	virtual void OptimizeModel(SEData &data, SEModel &model, const ubyte verbose = 0)
	{
	}

};

#endif