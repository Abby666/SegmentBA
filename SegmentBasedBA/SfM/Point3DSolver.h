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

#ifndef _POINT_3D_SOLVER_H_
#define _POINT_3D_SOLVER_H_

#include "Point3DEstimatorData.h"
#include "Point3DEstimatorMinimalSample.h"
#include "Matrix3.h"

class Point3DSolver
{

public:

	bool Run(const Point3DEstimatorMinimalSample &data, Point3D &X, AlignedVector< ENFT_SSE::__m128> &work, const bool metric);
	bool Run(const Point3DEstimatorData &data, Point3D &X, AlignedVector< ENFT_SSE::__m128> &work, const bool metric);

private:

	LA::AlignedMatrix3f m_ATA;

};

#endif