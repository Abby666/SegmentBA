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

#ifndef _SEGMENT_SET_BUNDLE_ADJUSTOR_DATA_2D_SIMILARITY_H_
#define _SEGMENT_SET_BUNDLE_ADJUSTOR_DATA_2D_SIMILARITY_H_

#include "SequenceSet.h"
#include "BundleAdjustorData.h"
#include "Matrix2x7.h"
#include "Matrix7.h"
#include "Utility.h"

#define SEGMENT_SET_BA_ARGUMENT_2D_SIMILARITY SegmentIndex, SimilarityTransformation3D, LA::AlignedVector7f, LA::AlignedMatrix7f, \
											  TrackIndex, Point3D, LA::Vector3f, LA::SymmetricMatrix3f, \
											  MeasurementIndex, Point2D, LA::AlignedMatrix3x7f, MeasurementIndex, \
											  Camera::IntrinsicParameter, LA::Vector2f, LA::AlignedMatrix2f, LA::AlignedMatrix2x7f, LA::AlignedMatrix2x3f
class SegmentSetBundleAdjustorData2DSimilarity : public BundleAdjustorDataTemplate<SEGMENT_SET_BA_ARGUMENT_2D_SIMILARITY>
{

public:

	SegmentSetBundleAdjustorData2DSimilarity() : m_mapSegToMea(m_mapCamToMea), m_mapTrkToMea(m_mapPtToMea), m_mapMeaToSeg(m_mapMeaToCam), m_mapMeaToTrk(m_mapMeaToPt), 
		m_distortionInvalid(false) {}

	inline void InvalidateDistortion() { m_distortionInvalid = true; }

	virtual void Resize(const SegmentIndex &nSegs, const TrackIndex &nTrks, const MeasurementIndex &nMeas);
	virtual void ReorderMeasurements(const MeasurementIndexList &iMeasOriToNew);

	virtual float GetFactorSSEToMSE() const { return m_pSeqs[0]->GetIntrinsicMatrix().fxy() / m_xs.Size(); }
	virtual void ValidateGlobal();
	virtual void InvalidateGlobal();
	virtual bool IsGlobalValid() const;
	virtual void UndistortMeasurements(const Camera::IntrinsicParameter &Kr);
	virtual void ScaleMeasurements(const float scale, AlignedVector<Point2D> &xs);

	virtual void NormalizeData(const float dataNormalizeMedian);
	virtual void DenormalizeData();

	virtual double ComputeSSE(std::vector<float> &ptSSEs);
	virtual void ComputeMSE(std::vector<float> &seqMSEs);
	virtual double ComputeMSE(const Camera::IntrinsicParameter &Kr);
	//virtual double ComputeMSE(const TrackIndex &iTrk, const Point3D &X);
	virtual void ConstructNormalEquation(AlignedVector<LA::AlignedMatrix7f> &Dcs, AlignedVector<LA::SymmetricMatrix3f> &Dxs, LA::AlignedMatrix2f &Dg, 
		AlignedVector<LA::AlignedMatrix3x7f> &Wxcs, AlignedVector<LA::AlignedMatrix2x7f> &Wgcs, AlignedVector<LA::AlignedMatrix2x3f> &Wgxs, 
		AlignedVector<LA::AlignedVector7f> &bcs, AlignedVector<LA::Vector3f> &bxs, LA::Vector2f &bg, AlignedVector<LA::AlignedVector7f> &scs, 
		AlignedVector<LA::Vector3f> &sxs, LA::Vector2f &sg);
	virtual void ConstructNormalEquation(const AlignedVector<LA::AlignedVector7f> &scs, const AlignedVector<LA::Vector3f> &sxs, const LA::Vector2f &sg, 
		AlignedVector<LA::AlignedMatrix7f> &Dcs, AlignedVector<LA::SymmetricMatrix3f> &Dxs, LA::AlignedMatrix2f &Dg, AlignedVector<LA::AlignedMatrix3x7f> &Wxcs, 
		AlignedVector<LA::AlignedMatrix2x7f> &Wgcs, AlignedVector<LA::AlignedMatrix2x3f> &Wgxs, AlignedVector<LA::AlignedVector7f> &bcs, AlignedVector<LA::Vector3f> &bxs, 
		LA::Vector2f &bg);
	virtual void UpdateCameras(const AlignedVector<LA::AlignedVector7f> &scs, const AlignedVector<LA::AlignedVector7f> &xcs, 
		const AlignedVector<SimilarityTransformation3D> &SsOld);
	virtual void UpdatePoints(const AlignedVector<LA::Vector3f> &sxs, const AlignedVector<LA::Vector3f> &xxs, const AlignedVector<Point3D> &XsOld);
	virtual void UpdateGlobal(const LA::Vector2f &sg, const LA::Vector2f &xg, const Camera::IntrinsicParameter &Gold);

#if _DEBUG
	virtual void DebugSchurComplement(const float damping, const std::vector<std::pair<SegmentIndex, SegmentIndex> > &iPairs, 
		const AlignedVector<LA::AlignedMatrix7f> &Accs, const LA::AlignedMatrix2f &Agg, const AlignedVector<LA::AlignedMatrix2x7f> &Agcs, 
		const AlignedVector<LA::AlignedVector7f> &bcs, const LA::Vector2f &bg);
	virtual void DebugSolution(const float damping, const AlignedVector<LA::AlignedVector7f> &xcs, const AlignedVector<LA::Vector3f> &xxs, const LA::Vector2f &xg);
	virtual void DebugAx(const AlignedVector<LA::AlignedVector7f> &xcs, const LA::Vector2f &xg, const AlignedVector<LA::AlignedVector7f> &Axcs, const LA::Vector2f &Axg);
#endif

protected:

	MeasurementIndexList				&m_mapSegToMea;
	std::vector<MeasurementIndexList>	&m_mapTrkToMea;
	SegmentIndexList					&m_mapMeaToSeg;
	TrackIndexList						&m_mapMeaToTrk;
	SequenceFrameIndexList				m_mapMeaToSeqFrm, m_mapMeaToSeqFrmOri;
	SequenceFramePairIndexList			m_mapSegToSeqFrm;

	std::vector</*const */Sequence *> m_pSeqs;

	Point3D m_translation;
	float m_scaleScene, m_scaleFocal;
	std::vector<float> m_distSqs;
	AlignedVector<float> m_r2s, m_cs;
	AlignedVector<Point2D> m_r2xs, m_xus, m_xrs;

	bool m_distortionInvalid;

	friend SequenceSet;

};

#endif