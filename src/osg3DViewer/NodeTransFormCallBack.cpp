
#include "NodeTransFormCallBack.h"

#include "MainWindow.h"

osg::ref_ptr<osg::AnimationPathCallback> getPipePathCallBack(const std::vector<AnimationInfo>& infos)
{
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	double _currentPos = 0.0;
	double _currentAngleX = 0.0;

	double _currentAngleZ = 0.0;
	double _currentAngleY = 0.0;
	double _wheelRadius = 0.0;
	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		if (info.translate.z() != 0.0)
		{
			// 沿z轴前进
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0,_currentPos), 
				osg::Quat(0.0, osg::X_AXIS, osg::DegreesToRadians(_currentAngleY), osg::Y_AXIS, osg::DegreesToRadians(_currentAngleZ), osg::Z_AXIS)));

			_currentPos += info.translate.z();
			pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0,_currentPos), 
				osg::Quat(0.0, osg::X_AXIS, osg::DegreesToRadians(_currentAngleY), osg::Y_AXIS, osg::DegreesToRadians(_currentAngleZ), osg::Z_AXIS)));
		}
		//if (info.translate.x() != 0.0)
		//{
		//	// 沿z轴前进
		//	pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos,0.0, 0.0), 
		//		osg::Quat(0.0, osg::X_AXIS, osg::DegreesToRadians(_currentAngleY), osg::Y_AXIS, osg::DegreesToRadians(_currentAngleZ), osg::Z_AXIS)));

		//	_currentPos += info.translate.z();
		//	pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos,0.0, 0.0), 
		//		osg::Quat(0.0, osg::X_AXIS, osg::DegreesToRadians(_currentAngleY), osg::Y_AXIS, osg::DegreesToRadians(_currentAngleZ), osg::Z_AXIS)));
		//}
		else if (info.rotateY != 0.0)
		{
			// 沿Y轴旋转
			osg::Vec3 center;
			center = info.center3d;
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(center, 
				osg::Quat( osg::DegreesToRadians(_currentAngleY), osg::Vec3f(0.0,info.fdir,0.0))));

			_currentAngleY += info.rotateY;
			pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(center, 
				osg::Quat( osg::DegreesToRadians(_currentAngleY), osg::Vec3f(0.0,info.fdir,0.0))));

			//osg::AnimationPathCallback* nc = new osg::AnimationPathCallback(pAnimationPath);
			//nc->setPivotPoint(center);

			//return nc;
		}
		else if(info.rotateZ != 0.0)
		{
			// 沿Z轴旋转
			osg::Vec3 center;
			center = info.center3d;
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(center, 
				osg::Quat( osg::DegreesToRadians(_currentAngleZ), osg::Vec3f(0.0,0.0,1.0))));

			_currentAngleZ += info.rotateZ;
			pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(center, 
				osg::Quat( osg::DegreesToRadians(_currentAngleZ), osg::Vec3f(0.0,0.0,1.0))));

			//osg::AnimationPathCallback* nc = new osg::AnimationPathCallback(pAnimationPath);

			//nc->setPivotPoint(center);

			//return nc;

		}

	}	

	//mt->setUpdateCallback(new osg::AnimationPathCallback(pAnimationPath.get()));
	// return new osg::AnimationPathCallback(pAnimationPath);

	osg::Vec3 center;
	center = infos[0].center3d;
	osg::AnimationPathCallback* nc = new osg::AnimationPathCallback(pAnimationPath);
	nc->setPivotPoint(center);

	return nc;

}

osg::ref_ptr<osg::AnimationPathCallback> getModelRotateCallBack(const std::vector<AnimationInfo>& infos)
{
    osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
	osg::ref_ptr<osg::AnimationPathCallback> nc;
	std::vector<osg::ref_ptr<osg::AnimationPathCallback>> vnc;
	double _currentPos = 0.0;
	double _currentAngleX = 0.0;
	double _currentAngleY = 0.0;
	double _currentAngleZ = 0.0;
	double _lastAngleX = 0.0;
	double _lastAngleY = 0.0;
	double _lastAngleZ = 0.0;

	double _wheelRadius = 0.0;
	osg::Vec3 vecCenter = infos[0].center3d;

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		vecCenter = infos[i].center3d;
		//if(info.iAnimationType == 2)//旋转
		{
			if(info.rotateX != 0)
			{
				// 沿Y轴旋转
				if(i==0)
				{
					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
					osg::Quat( osg::DegreesToRadians(_lastAngleX), osg::Vec3f(1.0,0.0,0.0))));
				}
				else
				{
					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
						osg::Quat( osg::DegreesToRadians(_lastAngleX), osg::Vec3f(1.0,0.0,0.0))));
				}

				_currentAngleX += info.rotateX;
				_lastAngleX = _currentAngleX;
				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(vecCenter, 
					osg::Quat( osg::DegreesToRadians(_currentAngleX), osg::Vec3f(1.0,0.0,0.0))));
			}
			else if(info.rotateY != 0)
			{
				// 沿Y轴旋转
				if(i==0)
				{
					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
						osg::Quat( osg::DegreesToRadians(_lastAngleY), osg::Vec3f(0.0,-1.0,0.0))));
				}
				else
				{
					
					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
						osg::Quat( osg::DegreesToRadians(_lastAngleY), osg::Vec3f(0.0,-1.0,0.0))));

				}


				/*pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
					osg::Quat( osg::DegreesToRadians(_currentAngleY), osg::Vec3f(0.0,-1.0,0.0))));*/

				_currentAngleY += info.rotateY;
				_lastAngleY = _currentAngleY;
				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(vecCenter, 
					osg::Quat( osg::DegreesToRadians(_currentAngleY), osg::Vec3f(0.0,-1.0,0.0))));

			}
			else if(info.rotateZ != 0)
			{
				// 沿Z轴旋转
				if(i==0)
				{
					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
						osg::Quat( osg::DegreesToRadians(_lastAngleZ), osg::Vec3f(0.0,0.0,1.0))));
				}
				else
				{
					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
						osg::Quat( osg::DegreesToRadians(_lastAngleZ), osg::Vec3f(0.0,0.0,1.0))));
				}
				/*pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
				osg::Quat( osg::DegreesToRadians(_currentAngleZ), osg::Vec3f(0.0,0.0,1.0))));
				*/
				_currentAngleZ += info.rotateZ;
				_lastAngleZ = _currentAngleZ;
				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(vecCenter, 
					osg::Quat( osg::DegreesToRadians(_currentAngleZ), osg::Vec3f(0.0,0.0,1.0))));
			}
		}	
	}	

	nc = new osg::AnimationPathCallback(pAnimationPath);
	nc->setPivotPoint(vecCenter);
	return nc;
}

// X轴方向的平移加旋转
osg::ref_ptr<osg::AnimationPathCallback> getModelMoveRotateCallBack(const std::vector<AnimationInfo>& move,const std::vector<AnimationInfo>& rotate)
{
osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
	osg::ref_ptr<osg::AnimationPathCallback> nc;
	std::vector<osg::ref_ptr<osg::AnimationPathCallback>> vnc;

	double _lastPos_X = rotate[0].center3d.x();
	double _lastPos_Y = rotate[0].center3d.y();
	double _lastPos_Z = rotate[0].center3d.z();

	double _currentPos_X = 0.0;
	double _currentPos_Y = 0.0;
	double _currentPos_Z = 0.0;

	double _currentAngleX = 0.0;
	double _currentAngleY = 0.0;
	double _currentAngleZ = 0.0;
	double _lastAngleX = 0.0;
	double _lastAngleY = 0.0;
	double _lastAngleZ = 0.0;

	double _wheelRadius = 0.0;
	osg::Vec3 vecCenter = rotate[0].center3d;

	std::map<double,AnimationInfo> mapAnims;
	for (size_t i = 0; i < move.size();++i)
	{
		AnimationInfo info = move[i];
		mapAnims[info.fStartTime] = info;
	}

	for (size_t i = 0; i < rotate.size();++i)
	{
		AnimationInfo info = rotate[i];
		mapAnims[info.fStartTime] = info;
	}

	std::vector<AnimationInfo> infos;
	std::map<double,AnimationInfo>::iterator iter = mapAnims.begin();
	for (;iter != mapAnims.end();++iter)
	{
		infos.push_back(iter->second);
	}

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		//vecCenter = infos[i].center3d;
		////if(info.iAnimationType == 2)//旋转
		//{
		//	if(info.rotateX != 0)
		//	{
		//		// 沿Y轴旋转
		//		if(i==0)
		//		{
		//			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
		//			osg::Quat( osg::DegreesToRadians(_lastAngleX), osg::Vec3f(1.0,0.0,0.0))));
		//		}
		//		else
		//		{
		//			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
		//				osg::Quat( osg::DegreesToRadians(_lastAngleX), osg::Vec3f(1.0,0.0,0.0))));
		//		}

		//		_currentAngleX += info.rotateX;
		//		_lastAngleX = _currentAngleX;
		//		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(vecCenter, 
		//			osg::Quat( osg::DegreesToRadians(_currentAngleX), osg::Vec3f(1.0,0.0,0.0))));
		//	}
		//	else
		    if (info.rotateY == 0.0 && info.rotateZ == 0.0)
			{
				osg::Vec3 center = infos[0].center3d;
				//_lastAngleY = 90.0;

				// 平移
				if(i == 0)
				{
					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos_X, _lastPos_Y, _lastPos_Z), 
						osg::Quat(osg::DegreesToRadians(_lastAngleY), osg::Vec3f(0.0,-1.0,0.0))));
					_lastPos_X = 0;
					_lastPos_Y = 0;
					_lastPos_Z = 0;

				}
				else
				{
					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos_X, _lastPos_Y, _lastPos_Z), 
						osg::Quat(osg::DegreesToRadians(_lastAngleY), osg::Vec3f(0.0,-1.0,0.0))));
				}

				//_currentPos_X += info.translate.x();
				//_lastPos_X = _currentPos_X;
				_lastPos_X += info.translate.x();
				_lastPos_Y += info.translate.y();
				_lastPos_Z += info.translate.z();
				//_lastPos_X = center.x() + info.translate.x();
				//_lastPos_Y = center.y() + info.translate.y();
				//_lastPos_Z = center.z() + info.translate.z();

				//_lastPos_X = center.x()+_currentPos_X;
				//_lastPos_Y = center.y()+_currentPos_Y;
				//_lastPos_Z = center.z()+_currentPos_Z;

				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos_X, _lastPos_Y, _lastPos_Z), 
					osg::Quat( osg::DegreesToRadians(_lastAngleY), osg::Vec3f(0.0,-1.0,0.0))));
			}
		    else if(info.rotateY != 0.0)
			{
				osg::Vec3 center = info.center3d;
				// 沿Y轴旋转
				if(i==0)
				{
					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(/*osg::Vec3(_lastPos_X, _lastPos_Y, _lastPos_Z)*/center,
						osg::Quat( osg::DegreesToRadians(_lastAngleY), osg::Vec3f(0.0,-1.0,0.0))));
				}
				else
				{
					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(/*osg::Vec3(_lastPos_X, _lastPos_Y, _lastPos_Z)*/center,
						osg::Quat( osg::DegreesToRadians(_lastAngleY), osg::Vec3f(0.0,-1.0,0.0))));

				}


				/*pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
					osg::Quat( osg::DegreesToRadians(_currentAngleY), osg::Vec3f(0.0,-1.0,0.0))));*/

				_currentAngleY += info.rotateY;
				_lastAngleY = _currentAngleY;
				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(/*osg::Vec3(_lastPos_X, _lastPos_Y, _lastPos_Z)*/center, 
					osg::Quat( osg::DegreesToRadians(_currentAngleY), osg::Vec3f(0.0,-1.0,0.0))));


			}
            else if(info.rotateZ != 0.0)
			{
				osg::Vec3 center = info.center3d;
				// 沿Y轴旋转
				if(i==0)
				{
					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(center,
						osg::Quat( osg::DegreesToRadians(_lastAngleZ), osg::Vec3f(0.0,0.0,1.0))));
				}
				else
				{
					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(center,
						osg::Quat( osg::DegreesToRadians(_lastAngleZ), osg::Vec3f(0.0,0.0,1.0))));

				}

				_currentAngleZ += info.rotateZ;
				_lastAngleZ = _currentAngleZ;
				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(center, 
					osg::Quat( osg::DegreesToRadians(_currentAngleZ), osg::Vec3f(0.0,0.0,1.0))));

			}

			//else if(info.rotateZ != 0)
			//{
			//	// 沿Z轴旋转
			//	if(i==0)
			//	{
			//		pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
			//			osg::Quat( osg::DegreesToRadians(_lastAngleZ), osg::Vec3f(0.0,0.0,1.0))));
			//	}
			//	else
			//	{
			//		pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
			//			osg::Quat( osg::DegreesToRadians(_lastAngleZ), osg::Vec3f(0.0,0.0,1.0))));
			//	}
			//	/*pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
			//	osg::Quat( osg::DegreesToRadians(_currentAngleZ), osg::Vec3f(0.0,0.0,1.0))));
			//	*/
			//	_currentAngleZ += info.rotateZ;
			//	_lastAngleZ = _currentAngleZ;
			//	pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(vecCenter, 
			//		osg::Quat( osg::DegreesToRadians(_currentAngleZ), osg::Vec3f(0.0,0.0,1.0))));
			//}
		//}	
	}	

	nc = new osg::AnimationPathCallback(pAnimationPath);
	//vecCenter[0] += 113.0;
	nc->setPivotPoint(vecCenter);
	return nc;
}

osg::ref_ptr<osg::AnimationPathCallback> getModelSecondRotateCallBack(const std::vector<AnimationInfo>& infos)
{

	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
	osg::ref_ptr<osg::AnimationPathCallback> nc;
	std::vector<osg::ref_ptr<osg::AnimationPathCallback>> vnc;
	double _currentPos = 0.0;
	double _currentAngleX = 0.0;
	double _currentAngleY = 0.0;
	double _currentAngleZ = 0.0;
	double _lastAngleX = 0.0;
	double _lastAngleY = 0.0;
	double _lastAngleZ = 0.0;

	double _wheelRadius = 0.0;
	osg::Vec3 vecCenter = infos[0].center3d;

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];


		//if(info.iAnimationType == 2)//旋转
		{
			if(info.rotateX != 0)
			{
				// 沿Y轴旋转
				if(i==0)
				{
					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
					osg::Quat( osg::DegreesToRadians(_lastAngleX), osg::Vec3f(1.0,0.0,0.0))));
				}
				else
				{
					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
						osg::Quat( osg::DegreesToRadians(_lastAngleX), osg::Vec3f(1.0,0.0,0.0))));
				}

				_currentAngleX += info.rotateX;
				_lastAngleX = _currentAngleX;
				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(vecCenter, 
					osg::Quat( osg::DegreesToRadians(_currentAngleX), osg::Vec3f(1.0,0.0,0.0))));
			}
			else if(info.rotateY != 0)
			{
				// 沿Y轴旋转
				if(i==0)
				{
					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
						osg::Quat( osg::DegreesToRadians(_lastAngleY), osg::Vec3f(0.0,-1.0,0.0))));
				}
				else
				{
					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
						osg::Quat( osg::DegreesToRadians(_lastAngleY), osg::Vec3f(0.0,-1.0,0.0))));
				}


				/*pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
					osg::Quat( osg::DegreesToRadians(_currentAngleY), osg::Vec3f(0.0,-1.0,0.0))));*/

				_currentAngleY += info.rotateY;
				_lastAngleY = _currentAngleY;
				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(vecCenter, 
					osg::Quat( osg::DegreesToRadians(_currentAngleY), osg::Vec3f(0.0,-1.0,0.0))));

			}
			else if(info.rotateZ != 0)
			{
				// 沿Z轴旋转
				if(i==0)
				{
					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
						osg::Quat( osg::DegreesToRadians(_lastAngleZ), osg::Vec3f(0.0,0.0,1.0))));
				}
				else
				{
					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
						osg::Quat( osg::DegreesToRadians(_lastAngleZ), osg::Vec3f(0.0,0.0,1.0))));
				}
				/*pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
				osg::Quat( osg::DegreesToRadians(_currentAngleZ), osg::Vec3f(0.0,0.0,1.0))));
				*/
				_currentAngleZ += info.rotateZ;
				_lastAngleZ = _currentAngleZ;
				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(vecCenter, 
					osg::Quat( osg::DegreesToRadians(_currentAngleZ), osg::Vec3f(0.0,0.0,1.0))));
			}
		}	
	}	

	nc = new osg::AnimationPathCallback(pAnimationPath);
	nc->setPivotPoint(vecCenter);
	return nc;
}

osg::ref_ptr<osg::AnimationPathCallback> getModelMoveCallBackNewNew(const std::vector<AnimationInfo>& infos)
{
	double _lastPos_X = 0.0;
	double _currentPos_X = 0.0;
	double _lastPos_Y = 0.0;
	double _currentPos_Y = 0.0;
	double _lastPos_Z = 0.0;
	double _currentPos_Z = 0.0;

	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		osg::Vec3d beginPos = info.absolutePos - osg::Vec3d(info.translate.x(),info.translate.y(),info.translate.z());

		if(i == 0)
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(beginPos, 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
			_lastPos_X = 0;
			_lastPos_Y = 0;
			_lastPos_Z = 0;

		}
		else
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos_X, _lastPos_Y, _lastPos_Z), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
		}

		//_currentPos_X += info.translate.x();
		//_lastPos_X = _currentPos_X;
		_currentPos_X += info.translate.x();
		_currentPos_Y += info.translate.y();
		_currentPos_Z += info.translate.z();
		_lastPos_X = _currentPos_X;
		_lastPos_Y = _currentPos_Y;
		_lastPos_Z = _currentPos_Z;

		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos_X, _currentPos_Y, _currentPos_Z), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
	}

	//创建更新动画回调
	return new osg::AnimationPathCallback(pAnimationPath.get());
}

osg::ref_ptr<osg::AnimationPathCallback> getModelMoveCallBackNew(const std::vector<AnimationInfo>& infos)
{
	double _lastPos_X = 0.0;
	double _currentPos_X = 0.0;
	double _lastPos_Y = 0.0;
	double _currentPos_Y = 0.0;
	double _lastPos_Z = 0.0;
	double _currentPos_Z = 0.0;

	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		if(i == 0)
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
			_lastPos_X = 0;
			_lastPos_Y = 0;
			_lastPos_Z = 0;

		}
		else
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos_X, _lastPos_Y, _lastPos_Z), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
		}

			//_currentPos_X += info.translate.x();
			//_lastPos_X = _currentPos_X;
		_currentPos_X += info.translate.x();
		_currentPos_Y += info.translate.y();
		_currentPos_Z += info.translate.z();
		_lastPos_X = _currentPos_X;
		_lastPos_Y = _currentPos_Y;
		_lastPos_Z = _currentPos_Z;

		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos_X, _currentPos_Y, _currentPos_Z), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
	}

	//创建更新动画回调
	return new osg::AnimationPathCallback(pAnimationPath.get());
}

//osg::ref_ptr<osg::AnimationPathCallback> getLongStaightPipePathCallBack(const std::vector<AnimationInfo>& infos)
//{
//
//}

osg::ref_ptr<osg::AnimationPathCallback> getModelMoveCallBack(const std::vector<AnimationInfo>& infos)
{
	double _lastPos_X = 0.0;
	double _currentPos_X = 0.0;
	double _lastPos_Y = 0.0;
	double _currentPos_Y = 0.0;
	double _lastPos_Z = 0.0;
	double _currentPos_Z = 0.0;

	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

 	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		if(info.translate.x() != 0)
		{
			if(i == 0)
			{
				pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
				_lastPos_X = 0;
				_lastPos_Y = 0;
				_lastPos_Z = 0;

			}
			else
			{
				pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos_X, _lastPos_Y, _lastPos_Z), 
					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
			}

			//_currentPos_X += info.translate.x();
			//_lastPos_X = _currentPos_X;
			_currentPos_X += info.translate.x();
			_currentPos_Y += info.translate.y();
			_currentPos_Z += info.translate.z();
			_lastPos_X = _currentPos_X;
			_lastPos_Y = _currentPos_Y;
			_lastPos_Z = _currentPos_Z;
			pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos_X, _currentPos_Y, _currentPos_Z), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
		}
		if(info.translate.y() != 0)
		{
			if(i == 0)
			{
				pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
				_lastPos_X = 0;
				_lastPos_Y = 0;
				_lastPos_Z = 0;
			}
			else
			{
				pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos_X, _lastPos_Y, _lastPos_Z), 
					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
			}

			//_currentPos_Y += info.translate.y();
			//_lastPos_Y = _currentPos_Y;
			_currentPos_X += info.translate.x();
			_currentPos_Y += info.translate.y();
			_currentPos_Z += info.translate.z();
			_lastPos_X = _currentPos_X;
			_lastPos_Y = _currentPos_Y;
			_lastPos_Z = _currentPos_Z;
			pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos_X, _currentPos_Y, _currentPos_Z), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
		}
		if(info.translate.z() != 0)
		{
			if(i == 0)
			{
				pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
				_lastPos_X = 0;
				_lastPos_Y = 0;
				_lastPos_Z = 0;
			}
			else
			{
				pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos_X, _lastPos_Y, _lastPos_Z), 
					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
			}

			//_currentPos_Z += info.translate.z();
			//_lastPos_Z = _currentPos_Z;
			_currentPos_X += info.translate.x();
			_currentPos_Y += info.translate.y();
			_currentPos_Z += info.translate.z();
			_lastPos_X = _currentPos_X;
			_lastPos_Y = _currentPos_Y;
			_lastPos_Z = _currentPos_Z;
			pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos_X, _currentPos_Y, _currentPos_Z), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
		}
	}

	//创建更新动画回调
	return new osg::AnimationPathCallback(pAnimationPath.get());
}

osg::ref_ptr<osg::AnimationPathCallback> getCheTouCallback(const std::vector<AnimationInfo>& infos)
{
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	double _currentPos = 0.0;
	double _currentAngleZ = 0.0;
	double _wheelRadius = 0.0;
	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		if(info.rotateZ!= 0)
		{
			// 沿Z轴旋转
			osg::Vec3 center;
			center = info.center3d;
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(center, 
				osg::Quat( osg::DegreesToRadians(_currentAngleZ), osg::Vec3f(0.0,0.0,1.0))));

			_currentAngleZ += info.rotateZ;
			pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(center, 
				osg::Quat( osg::DegreesToRadians(_currentAngleZ), osg::Vec3f(0.0,0.0,1.0))));


			osg::AnimationPathCallback* nc = new osg::AnimationPathCallback(pAnimationPath);

			nc->setPivotPoint(center);

			return nc;

		}
	}	

	//mt->setUpdateCallback(new osg::AnimationPathCallback(pAnimationPath.get()));
	return new osg::AnimationPathCallback(pAnimationPath);
}

osg::ref_ptr<osg::AnimationPathCallback> getCarPathCallback(const std::vector<AnimationInfo>& infos)
{
	double _currentPos = 0.0;
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];
		pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0,_currentPos), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));

		_currentPos += info.translate.z();
		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0,_currentPos), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
	}

	//创建更新动画回调
	return new osg::AnimationPathCallback(pAnimationPath.get());
}

osg::ref_ptr<osg::AnimationPathCallback> getJiTouWanGuan1PathCallback(const std::vector<AnimationInfo>& infos)
{

	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
	osg::ref_ptr<osg::AnimationPathCallback> nc;
	std::vector<osg::ref_ptr<osg::AnimationPathCallback>> vnc;
	double _currentPos = 0.0;
	double _currentAngleY = 0.0;
	double _wheelRadius = 0.0;
	osg::Vec3 vecCenter = infos[0].center3d;

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		if(info.rotateY != 0)
		{
			// 沿Y轴旋转
			osg::Vec3 center;
			center = info.center3d;
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
				osg::Quat( osg::DegreesToRadians(_currentAngleY), osg::Vec3f(0.0,-1.0,0.0))));

			_currentAngleY += info.rotateY;
			pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(vecCenter, 
				osg::Quat( osg::DegreesToRadians(_currentAngleY), osg::Vec3f(0.0,-1.0,0.0))));

		}

	}	

	nc = new osg::AnimationPathCallback(pAnimationPath);
	nc->setPivotPoint(vecCenter);
	return nc;
}

osg::ref_ptr<osg::AnimationPathCallback> getJiTouWanGuan2PathCallback(const std::vector<AnimationInfo>& infos)
{

	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
	osg::ref_ptr<osg::AnimationPathCallback> nc;
	std::vector<osg::ref_ptr<osg::AnimationPathCallback>> vnc;
	double _currentPos = 0.0;
	double _currentAngleY = 0.0;
	double _wheelRadius = 0.0;
	osg::Vec3 vecCenter = infos[0].center3d;

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		if(info.rotateY != 0)
		{
			// 沿Y轴旋转
			osg::Vec3 center;
			center = info.center3d;
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
				osg::Quat( osg::DegreesToRadians(_currentAngleY), osg::Vec3f(0.0,-1.0,0.0))));

			_currentAngleY += info.rotateY;
			pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(vecCenter, 
				osg::Quat( osg::DegreesToRadians(_currentAngleY), osg::Vec3f(0.0,-1.0,0.0))));

		}

	}	

	nc = new osg::AnimationPathCallback(pAnimationPath);
	nc->setPivotPoint(vecCenter);
	return nc;
}

osg::ref_ptr<osg::AnimationPathCallback> getZhuaLiaoMoPathCallback(const std::vector<AnimationInfo>& infos)
{
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	double _currentPos = 0.0;
	double _currentAngleZ = 0.0;
	double _wheelRadius = 0.0;
	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		if(info.rotateZ != 0)
		{
			// 沿Z轴旋转
			osg::Vec3 center;
			center = info.center3d;
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(center, 
				osg::Quat( osg::DegreesToRadians(_currentAngleZ), osg::Vec3f(0.0,0.0,1.0))));

			_currentAngleZ += info.rotateZ;
			pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(center, 
				osg::Quat( osg::DegreesToRadians(_currentAngleZ), osg::Vec3f(0.0,0.0,1.0))));


			osg::AnimationPathCallback* nc = new osg::AnimationPathCallback(pAnimationPath);

			nc->setPivotPoint(center);

			return nc;

		}

	}	

	return new osg::AnimationPathCallback(pAnimationPath);
}

osg::ref_ptr<osg::AnimationPathCallback> getBackPipePathCallBack(const std::vector<AnimationInfo>& infos)
{
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	double _currentPos = 0.0;
	double _currentAngleX = 0.0;
	double _currentAngleY = 0.0;
	double _wheelRadius = 0.0;
	double _lastPos = 0.0;
	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];
		if (info.translate.z() != 0.0)
		{
			if(i == 0)
			{
				pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
					osg::Quat(0.0, osg::X_AXIS, 0.0,  osg::X_AXIS, 0.0, osg::Z_AXIS)));
				_lastPos = 0;
			}
			else
			{
				pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3( 0.0, 0.0,_lastPos), 
					osg::Quat(0.0, osg::X_AXIS, 0.0,  osg::X_AXIS, 0.0, osg::Z_AXIS)));
			}


			////// 沿x轴前进
			//pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3( 0.0, 0.0,_currentPos), 
			//	osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));

			_currentPos += info.translate.z();
			_lastPos += info.translate.z();
			pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0,_currentPos), 
				osg::Quat(0.0, osg::X_AXIS,0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
		}		
	}	

	return new osg::AnimationPathCallback(pAnimationPath);
}

osg::ref_ptr<osg::AnimationPathCallback> getLongStaightPipePathCallBack(const std::vector<AnimationInfo>& infos)
{
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	double _currentPos = 0.0;
	double _currentAngleX = 0.0;
	double _currentAngleY = 0.0;
	double _wheelRadius = 0.0;
	double _lastPos = 0.0;
	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];
	}	

	return new osg::AnimationPathCallback(pAnimationPath);

#if 0
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	double _currentPos = 0.0;
	double _currentAngleX = 0.0;
	double _currentAngleY = 0.0;
	double _wheelRadius = 0.0;
	double _lastPos = 0.0;
	for (int i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];
		if (info.translate.z() != 0.0)
		{
			if(i == 0)
			{
				pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
					osg::Quat(0.0, osg::X_AXIS, 0.0,  osg::X_AXIS, 0.0, osg::Z_AXIS)));
				_lastPos = 0;
			}
			else
			{
				pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3( 0.0, 0.0,_lastPos), 
					osg::Quat(0.0, osg::X_AXIS, 0.0,  osg::X_AXIS, 0.0, osg::Z_AXIS)));
			}


			////// 沿x轴前进
			//pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3( 0.0, 0.0,_currentPos), 
			//	osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));

			_currentPos += info.translate.z();
			_lastPos += info.translate.z();
			pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0,_currentPos), 
				osg::Quat(0.0, osg::X_AXIS,0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
		}		
	}	

	return new osg::AnimationPathCallback(pAnimationPath);
#endif
}

osg::ref_ptr<osg::AnimationPathCallback> getJitouWanGuan3CallBack(const std::vector<AnimationInfo>& infos)
{
	double _lastPos = 0.0;
	double _currentPos = 0.0;
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		if(i == 0)
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
			_lastPos = 0;
		}
		else
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
		}

		_currentPos += info.translate.x();
		_lastPos = _currentPos;
		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
	}

	//创建更新动画回调
	return new osg::AnimationPathCallback(pAnimationPath.get());
}

osg::ref_ptr<osg::AnimationPathCallback> getWanQuMo2CallBack(const std::vector<AnimationInfo>& infos)
{
	double _lastPos = 0.0;
	double _currentPos = 0.0;
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		if(i == 0)
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
			_lastPos = 0;
		}
		else
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
		}

		_currentPos += info.translate.x();
		_lastPos = _currentPos;
		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
	}

	//创建更新动画回调
	return new osg::AnimationPathCallback(pAnimationPath.get());

}

osg::ref_ptr<osg::AnimationPathCallback> getZhuJia2PathCallBack(const std::vector<AnimationInfo>& infos)
{

	double _lastPos = 0.0;
	double _currentPos = 0.0;
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		if(i == 0)
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
			_lastPos = 0;
		}
		else
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
		}

		_currentPos += info.translate.x();
		_lastPos = _currentPos;
		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
	}

	//创建更新动画回调
	return new osg::AnimationPathCallback(pAnimationPath.get());

}

osg::ref_ptr<osg::AnimationPathCallback> getZhuJia1PathCallBack(const std::vector<AnimationInfo>& infos)
{
	double _currentPosY = 0.0;
	double _currentPosZ = 0.0;
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];
		pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, _currentPosY, _currentPosZ), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));

		_currentPosY += info.translate.y();
		_currentPosZ += info.translate.z();

		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, _currentPosY, _currentPosZ), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
	}

	//创建更新动画回调
	return new osg::AnimationPathCallback(pAnimationPath.get());

}

osg::ref_ptr<osg::AnimationPathCallback> getDaoJia2PathCallBack(const std::vector<AnimationInfo>& infos)
{
	double      _currentPos = 0.0;
	double      _lastPos = 0.0;
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		if(i == 0)
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
			_lastPos = 0;
		}
		else
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
		}

		_currentPos += info.translate.x();
		_lastPos = _currentPos;
		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
	}

	//创建更新动画回调
	return new osg::AnimationPathCallback(pAnimationPath.get());
}

osg::ref_ptr<osg::AnimationPathCallback> getDaoJia2_1PathCallBack(const std::vector<AnimationInfo>& infos)
{
	double      _currentPos = 0.0;
	double      _lastPos = 0.0;
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		if(i == 0)
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
			_lastPos = 0;
		}
		else
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
		}

		_currentPos += info.translate.x();
		_lastPos = _currentPos;
		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
	}

	//创建更新动画回调
	return new osg::AnimationPathCallback(pAnimationPath.get());
}

osg::ref_ptr<osg::AnimationPathCallback> getDaoJia1PathCallBack(const std::vector<AnimationInfo>& infos)
{
	double  _currentPos = 0.0;
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];
		pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, _currentPos), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));

		_currentPos += info.translate.z();
		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, _currentPos), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
	}

	//创建更新动画回调
	return new osg::AnimationPathCallback(pAnimationPath.get());
}

osg::ref_ptr<osg::AnimationPathCallback> getDaoJia1_1PathCallBack(const std::vector<AnimationInfo>& infos)
{
	double  _currentPos = 0.0;
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];
		pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, _currentPos), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));

		_currentPos += info.translate.z();
		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, _currentPos), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
	}

	//创建更新动画回调
	return new osg::AnimationPathCallback(pAnimationPath.get());
}

osg::ref_ptr<osg::AnimationPathCallback> getLDaoJia0PathCallBack(const std::vector<AnimationInfo>& infos)
{
	double  _currentPos = 0.0;
	double  _lastPos = 0.0;

	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		if(i == 0)
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
			_lastPos = 0;
		}
		else
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
		}

		_currentPos += info.translate.x();
		_lastPos = _currentPos;
		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
	}

	//创建更新动画回调
	return new osg::AnimationPathCallback(pAnimationPath.get());
}

osg::ref_ptr<osg::AnimationPathCallback> getLDaoJia1PathCallBack(const std::vector<AnimationInfo>& infos)
{
	double      _currentPos = 0.0;
	double      _lastPos = 0.0;
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		if(i == 0)
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
			_lastPos = 0;
		}
		else
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
		}

		_currentPos += info.translate.x();
		_lastPos = _currentPos;
		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
	}

	//创建更新动画回调
	return new osg::AnimationPathCallback(pAnimationPath.get());
}

osg::ref_ptr<osg::AnimationPathCallback> getLDaoJia2PathCallBack(const std::vector<AnimationInfo>& infos)
{
	double      _currentPos = 0.0;
	double      _lastPos = 0.0;
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		if(i == 0)
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
			_lastPos = 0;
		}
		else
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
		}

		_currentPos += info.translate.x();
		_lastPos = _currentPos;
		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
	}

	//创建更新动画回调

	return new osg::AnimationPathCallback(pAnimationPath.get());
}

osg::ref_ptr<osg::AnimationPathCallback> getLDaoJia3PathCallBack(const std::vector<AnimationInfo>& infos)
{
	double      _currentPos = 0.0;
	double      _lastPos = 0.0;
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		if(i == 0)
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
			_lastPos = 0;
		}
		else
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
		}

		_currentPos += info.translate.x();
		_lastPos = _currentPos;
		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
	}

	//创建更新动画回调

	return new osg::AnimationPathCallback(pAnimationPath.get());
}

osg::ref_ptr<osg::AnimationPathCallback> getRDaoJia0PathCallBack(const std::vector<AnimationInfo>& infos)
{
	double      _currentPos = 0.0;
	double      _lastPos = 0.0;
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		if(i == 0)
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
			_lastPos = 0;
		}
		else
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
		}

		_currentPos += info.translate.x();
		_lastPos = _currentPos;
		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
	}

	//创建更新动画回调
	return new osg::AnimationPathCallback(pAnimationPath.get());
}

osg::ref_ptr<osg::AnimationPathCallback> getRDaoJia1PathCallBack(const std::vector<AnimationInfo>& infos)
{
	double      _currentPos = 0.0;
	double      _lastPos = 0.0;
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		if(i == 0)
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
			_lastPos = 0;
		}
		else
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
		}

		_currentPos += info.translate.x();
		_lastPos = _currentPos;
		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
	}

	//创建更新动画回调
	return new osg::AnimationPathCallback(pAnimationPath.get());
}

osg::ref_ptr<osg::AnimationPathCallback> getRDaoJia2PathCallBack(const std::vector<AnimationInfo>& infos)
{
	double      _currentPos = 0.0;
	double      _lastPos = 0.0;
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		if(i == 0)
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
			_lastPos = 0;
		}
		else
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
		}

		_currentPos += info.translate.x();
		_lastPos = _currentPos;
		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
	}

	//创建更新动画回调
	return new osg::AnimationPathCallback(pAnimationPath.get());
}

osg::ref_ptr<osg::AnimationPathCallback> getRDaoJia3PathCallBack(const std::vector<AnimationInfo>& infos)
{

	double      _currentPos = 0.0; 
	double      _lastPos = 0.0;
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];

		if(i == 0)
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
			_lastPos = 0;
		}
		else
		{
			pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
		}

		_currentPos += info.translate.x();
		_lastPos = _currentPos;
		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
	}

	//创建更新动画回调
	return new osg::AnimationPathCallback(pAnimationPath.get());
}

osg::ref_ptr<osg::AnimationPathCallback> getZuoFuTuiPathCallBack(const std::vector<AnimationInfo>& infos)
{
	double      _currentPosX = 0.0;
	double      _currentPosY = 0.0;
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];
		pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPosX, _currentPosY, 0.0), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));

		_currentPosX += info.translate.x();
		_currentPosY += info.translate.x();


		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPosX, _currentPosY, 0.0), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
	}

	//创建更新动画回调
	return new osg::AnimationPathCallback(pAnimationPath.get());
}

osg::ref_ptr<osg::AnimationPathCallback> getYouFuTuiPathCallBack(const std::vector<AnimationInfo>& infos)
{

	double      _currentPosX = 0.0;
	double      _currentPosY = 0.0;

	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];
		pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPosX, _currentPosY, 0.0), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));

		_currentPosX += info.translate.x();
		_currentPosY += info.translate.x();


		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPosX, _currentPosY, 0.0), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
	}

	//创建更新动画回调
	return new osg::AnimationPathCallback(pAnimationPath.get());
}

osg::ref_ptr<osg::AnimationPathCallback> getJiTouMoZuoPathCallBack(const std::vector<AnimationInfo>& infos)
{
	double      _currentPos = 0.0;
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];
		pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos,0.0, 0.0), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));

		_currentPos += info.translate.x();
		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos,0.0, 0.0), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
	}

	return new osg::AnimationPathCallback(pAnimationPath.get());
}

osg::ref_ptr<osg::AnimationPathCallback> getJiTouDiZuoPathCallBack(const std::vector<AnimationInfo>& infos)
{
	double      _currentPos = 0.0; 
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (size_t i = 0; i < infos.size();++i)
	{
		const AnimationInfo& info = infos[i];
		pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, _currentPos,0.0 ), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));

		_currentPos += info.translate.y();
		pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, _currentPos, 0.0), 
			osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
	}

	return new osg::AnimationPathCallback(pAnimationPath.get());
}

PipeInfoCallback::PipeInfoCallback(double s,double e,int i,int iTw,bool Istw, MainWindow* window,bool autoplan)
	: m_TimeStart(s)
	, m_TimeEnd(e)
	, m_iCurrent(i)
	, m_iCurrentTw(iTw)
	, m_bIsTw(Istw)
	, m_window(window)
	, m_bFinished(false)
	, m_bAutoPlan(autoplan)
{
}

void PipeInfoCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	TextTransform *mt = dynamic_cast<TextTransform*>(node);
	if (mt)
	{
		osg::AnimationPathCallback* back = dynamic_cast<osg::AnimationPathCallback*>(mt->getUpdateCallback());
		if (back)
		{
			double time = back->getAnimationTime();
			bool isPause = back->getPause();
			HUDText* hud = mt->GetHUDText();
			Table* tab = mt->GetTextTable();
			Table* twtable = mt->GetTWTable();
			if (tab!=NULL && !isPause && time>=m_TimeStart && time<m_TimeEnd /*&& !m_bIsTw*/)
			{
				tab->SetRowItemsColor(m_iCurrent);
				tab->AddiText(m_iCurrent);

				//if (twtable!=NULL)
				//{
				//	twtable->ResetAllColor();
				//}
			}	

			if (twtable!=NULL && !isPause && time>=m_TimeStart && time<m_TimeEnd /*&& m_bIsTw*/)
			{
				twtable->SetRowItemsColor(m_iCurrentTw);

				//if (tab!=NULL)
				//{
				//	tab->ResetAllColor();
				//}
			}

			if (m_iCurrent==(tab->rowCount()-2) && time > m_TimeEnd && !m_bFinished)
			{
				// 动画结束
				LogInfo("animation end !");

				if (m_window)
				{
					m_window->closeTimer();
				}

				if (m_bAutoPlan)
				{
					m_window->WriteProductionFile("");
				}

				m_bFinished = true;
			}
		}
	}

	traverse(node, nv);
}