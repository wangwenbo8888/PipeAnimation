
/******************************************************************************
   osGraphX: a 3D file viewer
   Copyright(C) 2011-2012  xbee@xbee.net

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *******************************************************************************/

#ifndef _XOBJECT_LOADER_H_
#define _XOBJECT_LOADER_H_

#include <QtCore/QObject>

#include <osg/Node>
#include <QColor>
#include <osg/Material>
struct Vertex //¶¥µã½á¹¹
{
	Vertex() {}
	Vertex(float x, float y, float z)
	{
		_x = x;  _y = y;  _z = z;
	}
	float _x, _y, _z;
};


class xObjectLoader : public QObject
{
    Q_OBJECT

public:
    xObjectLoader(QObject *parent = 0);
    virtual ~xObjectLoader(){}
	osg::ref_ptr<osg::Node> createQuade(std::string strFileName);
	QColor GetUniqueColor();
	bool WriteBinarySTL(const osg::Vec3Array& vs , const osg::Vec3Array& ns, const std::string& sFileName);
	osg::ref_ptr<osg::Node> GetLoadedModel(){return m_ptrLoadedModel;}
	osg::ref_ptr<osg::Material> GetMaterial(){return m_pMaterial;}

	QString GetFileName(){return m_file_name;}
	osg::Vec4 GetLoadModelColor();

	void CreatAlterSTLNodeByFile(QString stlpath);
	//osg::ref_ptr<osg::Node> GetCurGeode();
public slots:
    void slotNewObjectToLoad(const QString &);
    void slotSetOptimization(bool val) {m_bOptimize = val; }

signals:
	void sigNewObjectToView(osg::ref_ptr<osg::Node> node,
		osg::ref_ptr<osg::Material> material, const QString& name);

private:
    QString m_file;
    std::vector<osg::ref_ptr<osg::Node>> m_ptrLoadedModels;
    bool m_bOptimize;
	//QColor m_color;
	osg::Vec4 m_color;
	std::vector<QColor>m_vColor;
	osg::ref_ptr<osg::Node> m_ptrLoadedModel;
	QString  m_file_name;
	//osg::ref_ptr<osg::Geode> m_geode;
	osg::ref_ptr<osg::Material> m_pMaterial;

};

#endif // _SCREENSHOT_HANDLER_H_
