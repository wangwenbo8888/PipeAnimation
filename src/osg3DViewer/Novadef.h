		#ifndef _XJDEF_H_
#define _XJDEF_H_

//#include "Common.h"

#include <cmath>
#include <string>

#include "NovaPoint3D.h"

#include <qstring.h>

#include <osg/Array>
#include <osg/Group>
#include <osg/Geode>

#include "Pipe.h"

class PIPE ;
//class TextTransform : public osg::PositionAttitudeTransform;

//#include "MainWindow.h"

using namespace std;

typedef unsigned char  XJBYTE;

typedef unsigned long  COLORREF;

typedef int BOOL;

//变量名转化为字符串
#define VarToName(varname) (#varname) 

#ifndef TRUE
#define TRUE   1
#define FALSE  0
#endif

#ifndef Pi
	#define Pi   3.14159265358979324
#endif
/*
 * Key State Masks for Mouse Messages
 */
/*
#define MK_LBUTTON          0x0001
#define MK_RBUTTON          0x0002
#define MK_SHIFT            0x0004
#define MK_CONTROL          0x0008
#define MK_MBUTTON          0x0010
#define MK_WHELL            0x0012
*/
//定义宏，表示对象的ID号
#define XJ_OMKEY_BASE  10000 
#define XJ_GROUP_SUBOBJECT_OMKEY_BASE 0
#define XJ_BONG_SHOW_GROUP_ID 100000000

#define XJ_SMALL           0.000001
#define XJ_EXPONENTIAL     1.0e-10
//使用chai3d库
#define C_USE_CHAI3D


//网格对象默认颜色
#define DefaultColorMesh RGB(0,191,255)

//设置模型透明时，给定的默认透明度
#define DEFAULT_TRANSPARENT 0.6f


//定义一个Undo/Redo的枚举类型
typedef enum
{
	XJ_COMMAND_ADD,
	XJ_COMMAND_UPDATE,	     //对象整体更新，适用于网格算法中
	XJ_COMMAND_DELETE,
	XJ_COMMAND_PROPERTY_UPDATE,  //对象属性更新,下面包含具体的属性
	XJ_COMMAND_VISIBLE,
	XJ_COMMAND_POSITION,
	XJ_COMMAND_NAME,
	XJ_COMMAND_TRANSPARENT,
	XJ_COMMAD_NOUNDOREDO

} EXJCommandType;


//OSG视图更新属性类型
enum EOSGUpdateType
{
	UPDATE_ALL,
	UPDATE_COLOR,
	UPDATE_POSITION,     //更新对象位置
	UPDATE_TRANSPARENT,  //更新对象透明度
	UPDATE_VISIBLE,	   //更新对象显示隐藏
	UPDATE_WIREFRAME,	   //更新对象线框显示
	UPDATE_DELETE		   //删除对象显示
};

//HUD在窗口的位置
enum HUDTextWinPos
{
	WINDOW_LEFT_TOP_POS = 0,
	WINDOW_RIGHT_TOP_POS,
	WINDOW_LEFT_BOTTOM_POS,
	WINDOW_RIGHT_BOTTOM_POS
};

//HUD在窗口的位置
typedef enum DisplayModel
{
	DISPLAY_POINT,
	DISPLAY_LINE,
	DISPLAY_FACE,
}EDisplayModel;
//文字对齐方式
typedef enum TextAlignmentType
{
	LEFT_TOP_ALIGNMENT,
	LEFT_CENTER_ALIGNMENT,
	LEFT_BOTTOM_ALIGNMENT,

	CENTER_TOP_ALIGNMENT,
	CENTER_CENTER_ALIGNMENT,
	CENTER_BOTTOM_ALIGNMENT,

	RIGHT_TOP_ALIGNMENT,
	RIGHT_CENTER_ALIGNMENT,
	RIGHT_BOTTOM_ALIGNMENT,

	LEFT_BASE_LINE_ALIGNMENT,
	CENTER_BASE_LINE_ALIGNMENT,
	RIGHT_BASE_LINE_ALIGNMENT,

	LEFT_BOTTOM_BASE_LINE_ALIGNMENT,
	CENTER_BOTTOM_BASE_LINE_ALIGNMENT,
	RIGHT_BOTTOM_BASE_LINE_ALIGNMENT,

	BASE_LINE_ALIGNMENT = LEFT_BASE_LINE_ALIGNMENT /// default.

} AlignmentType;

typedef enum XJObjectUpdateType
{
	XJ_OBJECT_UPDATE_TYPE_MODEL_CREATE,
	XJ_OBJECT_UPDATE_TYPE_MODEL_PREVIEW,
	XJ_OBJECT_UPDATE_TYPE_FEATURES,
	XJ_OBJECT_UPDATE_TYPE_MODEL_UPDATE,
	XJ_OBJECT_UPDATE_TYPE_MODEL_BOX,
	XJ_OBJECT_UPDATE_TYPE_MODEL_BOX_HIDE,

	XJ_OBJECT_UPDATE_TYPE_MODEL_COORD,
	XJ_OBJECT_UPDATE_TYPE_MODEL_COORD_OBJ,
	XJ_OBJECT_UPDATE_TYPE_MODEL_COORD_SCREEN,
	XJ_OBJECT_UPDATE_TYPE_MODEL_COORD_HIDE,

	XJ_OBJECT_UPDATE_TYPE_MODEL_CUT_PLANE_IN,
	XJ_OBJECT_UPDATE_TYPE_MODEL_CUT_PLANE_OUT,
	
	XJ_OBJECT_UPDATE_TYPE_MODEL_DELETE,

	XJ_OBJECT_UPDATE_TYPE_MODEL_AXIS,//  为了客户端显示轴线用 
	XJ_OBJECT_UPDATE_TYPE_NUMBER

} EXJObjectUpdateType;

typedef enum XJAfxMessageBoxType
{
	XJ_MB_NULL = 0,
	XJ_MB_OK,                       
	XJ_MB_OKCANCEL,                 
	XJ_MB_ABORTRETRYIGNORE,         
	XJ_MB_YESNOCANCEL,              
	XJ_MB_YESNO,                    
	XJ_MB_RETRYCANCEL             

}EXJAfxMessageBoxType;


typedef enum XJOperType
{
	/*三维视图交互,鼠标事件不带返回值 */
	XJ_VIEW_ORBIT,//旋转视图
	XJ_VIEW_PAN,//平移视图
	XJ_VIEW_ZOOM,//缩放视图
	XJ_VIEW_ZOOMALL,//总体视图查看
	XJ_VIEW_LEFT,// 左视图
	XJ_VIEW_RIGHT, // 右视图
	XJ_VIEW_UP, // 俯视图
	XJ_VIEW_DOWN, // 漾视图
	XJ_VIEW_FRONT, // 前视图
	XJ_VIEW_BACK, // 后视图
	XJ_VIEW_EDEG,	// 显示三角片边界
	/*三维模型交互   右键 响应*/
	//XJ_OPER_MOVE_WORLD_AXIS ,// 模型按世界坐标系 （坐标轴、环形框）进行位置转换
	//XJ_OPER_MOVE_OBJECT_AXIS ,// 模型按指定坐标系 （坐标轴、环形框）进行位置转换
	//XJ_OPER_MOVE_SCREEN_AXIS, //模型按屏幕坐标系 （坐标轴、环形框）进行位置转换
	/*选择对象类型*/
	XJ_OPER_SEL_LINE,
	XJ_OPER_SEL_PLANE,
	XJ_OPER_SEL_CIRCLE, 
	XJ_OPER_SEL_SPHERE,
	XJ_OPER_SEL_MESH,
	XJ_OPER_SEL_NUM
	/*测量类型*/
} EXJOperType;

typedef enum XJViewType
{
	OSG_VIEW_LEFT,// 左视图
	OSG_VIEW_RIGHT, // 右视图
	OSG_VIEW_TOP, // 上视图
	OSG_VIEW_BOTTOM, // 下视图
	OSG_VIEW_FRONT, // 前视图
	OSG_VIEW_BACK, // 后视图
	OSG_VIEW_HORITICAL,//水平
	OSG_VIEW_VERTICAL,//垂直
};


typedef enum XJProjectionPlane
{
	// 在不同投影面上的测量结果
	XJ_PROJECTION_XYZ = 0,   //	无投影平面
	XJ_PROJECTION_XOY = 1,	    //投影到XOY面上
	XJ_PROJECTION_XOZ = 2,	    //投影到XOZ面上
	XJ_PROJECTION_YOZ = 3,	  //投影到YOZ面上
	XJ_PROJECTION_USER = 4  //自定义投影平面

}EXJProjectionPlane;


typedef enum XJPrjType
{
	idNoType=0,
	idRestruction =1,//三维重构
	id3DPrint =2,//3D打印
	idTKA=3,//膝关节
	idTHA=4,//髋关节
	idFemurFar = 5,

}EXJPrjType;

typedef enum XJLocationType
{
	Left=0,
	Right =1,

}EXJLocationType;

typedef enum XJGuidePlaceType
{
	Side=0,
	Front =1,

}EXJGuidePlaceType;

typedef enum XJBoneType
{
	XJ_Femur=0,
	XJ_Tibia=1,
	XJ_Hip=2,

}EXJBoneType;


typedef enum XJDICOMType
{
	CT =0,
	MRI =1

}EXJDICOMType;

//模板设计中设计的类型
typedef enum XJGUIDEType
{
	FemurGuide,	 //股骨模板
	TibiaGuide,	 //胫骨模板
	HipGuide,    //髋关节模板
	HipSurgical, //髋关节术前规划

}EXJGUIDEType;

typedef enum XJSTATUSMESSAGE
{
   SELECT_ONE_MESH,
   SPHEERE_R,
   SPHERE_X,
   SPHERE_Y,
   SPHERE_Z,
   Point_Info,
   Vector_Info,
   Circle_Radius,
   Cylinder_Radius,
   Cylinder_PosDis,
   Cylinder_NegDis,
   Cone_FloorDis,
   Cone_TopDis,
   Cone_Radius,
   Cone_Draft,
   Spline_Create,
   Spline_Modify,
   Box_L,
   Box_W,
   Box_H,
   Box_FloorCenter,
   Box_HDir,
   Add_Object_Move,
   Add_Delete_Move,
   Close_OR_Return,
   MOVE,
   ROTATE,
   Rotate_Deg_Tip,
   Plane_L,
   Plane_W,
   Select_Object,
   Select_Mesh_Target,
   Select_Mesh_Tool,
   Select_Hole_Mesh,
   Select_Mesh,
   MakeShellDis,
   SelectMeshMerge,
   Merge_Delete_Mesh,
   Select_Spline,
   Display_Dis,
   Select_Prj_Plane,
   Off_Dis,
   Import_XJFile,
   Import_STL,
   Export_STL,
   Select_SPEC_TYPE,
   Select_Hip,
   Select_Pelvicum,
   Select_LeftRight,
   Set_OutDegree,
   Set_FrontDegree,
   SelectFemur,
   SelectFemurUp,
   SelectFibula,
   SelectTibia,
   SelectTibiaDown,
   SelectFemurCavityStart,
   SelectFemurCavityEnd,
   SelectFemurAPStart,
   SelectFemurAPEnd,
   SelectFemurTongKeStart,
   SelectFemurTongKeEnd,
   SelectFemurHouKeStart,
   SelectFemurHouKeEnd,
   SelectFemurFarStart,
   SelectFemurFarEnd,
   SelectTibiaInstallStart,
   SelectTibiaInstallEnd,
   SelectTibiaAPStart,
   SelectTibiaAPEnd,
   SelectTibiaMLStart,
   SelectTibiaMLEnd,
   SelectTibiaMDStart,
   SelectTibiaMDEnd,
   UploadingNag,
   UploadNagEnd,
   KneeFemurGuide,
   KneeTibiaGuide,
   HipImpGuide,
   SelectPoint,

   SYSTETM_INFORMATION,

   //算法
   ALGORITHM_BOOL_OPERATOR_SELECT_MESH_TARGET_1,
   ALGORITHM_BOOL_OPERATOR_SELECT_MESH_TARGET_2,
   ALGORITHM_BOOL_OPERATOR_FINISH,

   ALGORITHM_MERGE_ADD_OBJECTS,
   ALGORITHM_MERGE_FINISH,

   ALGORITHM_SEPARATE_SELECT_MESH,
   ALGORITHM_SEPARATE_DISCRETE_FINISH,

   ALGORITHM_SPLIT_SELECT_MESH,
   ALGORITHM_SPLIT_SELECT_SPLINE,
   ALGORITHM_SPLIT_FINISH,

   ALGORITHM_FILL_HOLE_SELECT_MESH,
   ALGORITHM_FILL_HOLE_SELECT_WAY,
   ALGORITHM_FILL_HOLE_FINISH,

   ALGORITHM_MAKE_SHELL_SELECT_MESH,
   ALGORITHM_MAKE_SHELL_SET_DIS,
   ALGORITHM_MAKE_SHELL_FINISH,

   ALGORITHM_OFFSET_SELECT_MESH,
   ALGORITHM_OFFSET_SET_DIS,
   ALGORITHM_OFFSET_FINISH,

   ALGORITHM_SMOOTH_SELECT_MESH,
   ALGORITHM_SMOOTH_SET_FACTOR,
   ALGORITHM_SMOOTH_FINISH,

   ALGORITHM_CUT_PLANE_SELECT_MESH,
   ALGORITHM_CUT_PLANE_SELECT_PROJECT_PLANE,
   ALGORITHM_CUT_PLANE_FINISH,

   ALGORITHM_REMOVE_DISCRETE_SELECT_MESH,
   ALGORITHM_REMOVE_DISCRETE_FINISH,

   ALGORITHM_FILTER_MESH_SELECT_MESH,
   ALGORITHM_FILTER_MESH_FACTOR,
   ALGORITHM_FILTER_MESH_FINISH,

   ALGORITHM_SUBDIVIDE_MESH_SELECT_MESH,
   ALGORITHM_SUBDIVIDE_MESH_FACTOR,
   ALGORITHM_SUBDIVIDE_MESH_FINISH,

   ALGORITHM_CUT_BY_HOOK_FACE_SELECT_MESH,
   ALGORITHM_CUT_BY_HOOK_FACE_SELECT_CUT_PLANE,
   ALGORITHM_CUT_BY_HOOK_FACE_FINISH,

   ALGORITHM_MIRROR_MESH_SELECT_MESH,
   ALGORITHM_MIRROR_MESH_SELECT_MIRROR_PLANE,
   ALGORITHM_MIRROR_MESH_FINISH,

   ALGORITHM_THREE_POINTS_REGIST_SELECT_MESH,
   ALGORITHM_THREE_POINTS_REGIST_SELECT_POINT,
   ALGORITHM_THREE_POINTS_REGIST_FINISH,

   ALGORITHM_INTERSECTION_SELECT_MESH1,
   ALGORITHM_INTERSECTION_SELECT_MESH2,
   ALGORITHM_INTERSECTION_FINISH,

   ALGORITHM_LINE_INTERSECTION_SELECT_MESH,
   ALGORITHM_LINE_INTERSECTION_SELECT_LINE,
   ALGORITHM_LINE_INTERSECTION_FINISH,

   ALGORITHM_STRETCH_SPLINE,
   ALGORITHM_STRETCH_DIRECTION,
   ALGORITHM_STRETCH_FINISH,

   ALGORITHM_PROJECTION_SPLINE,
   ALGORITHM_PROJECTION_PLANE,
   ALGORITHM_PROJECTION_FINISH,

   ALGORITHM_MESH_LETTING,
   //基础
   BASE_POINT_SET_TIPS,
   BASE_POINT_FINISH,

   BASE_LINE_SET_TIPS,
   BASE_LINE_FINISH,

   BASE_PLANE_SET_TIPS,
   BASE_PLANE_FINISH,

   BASE_BALL_SET_TIPS,
   BASE_BALL_FINISH,

   BASE_CYLINDER_SET_TIPS,
   BASE_CYLINDER_FINISH,

   BASE_CONE_SET_TIPS,
   BASE_CONE_FINISH,

   BASE_CURVE_SPLINE_SET_TIPS,
   BASE_CURVE_SPLINE_FINISH,

   BASE_MESH_CIRCLE_SET_TIPS,
   BASE_MESH_CIRCLE_FINISH,

   BASE_MESH_BOX_SET_TIPS,
   BASE_MESH_BOX_FINISH,

   BASE_MESH_BALL_SET_TIPS,
   BASE_MESH_BALL_FINISH,

   BASE_MESH_CYLINDER_SET_TIPS,
   BASE_MESH_CYLINDER_FINISH,

   BASE_MESH_CONE_SET_TIPS,
   BASE_MESH_CONE_FINISH,

    //测量
    MEASURE_DISTANCE_SET_TIPS,
    MEASURE_ANGLE_SET_TIPS,

	//设置
	CONFIGURE_MODEL_TRANSPARENCY,

    //数据库
    DATA_BASE_QUERY_SET_TIPS,
    DATA_BASE_QUERY_IMPORT_FINISH,

    DATA_BASE_DEVICE_SET_TIPS,
    DATA_BASE_DEVICE_IMPORT_FINISH,

    DATA_BASE_REGISTER_SET_TIPS,

    DATA_BASE_IMPORT_MESH_SET_TIPS,
    DATA_BASE_EXPORT_MESH_SET_TIPS,

    DATA_BASE_EXPORT_XJ_DOCUMENT_SET_TIPS,
    DATA_BASE_IMPORT_XJ_DOCUMENT_SET_TIPS,

    DATA_BASE_EXPORT_JPG_SET_TIPS,
    DATA_BASE_IMPORT_JPG_SET_TIPS,

	VIEW_COPY_ERROR,
	CLEAR_STATUS_BAR_INFO,

    //主页
    HOME_ROUGH_MODE_SET_TIPS,
    HOME_PRECISION_MODE_SET_TIPS,
    HOME_TRANSPARENT_SET_TIPS,
    HOME_UNTRANSPARENT_SET_TIPS,
    HOME_SHOW_SET_TIPS,
    HOME_HIDE_SET_TIPS,
    HOME_COLOR_SET_TIPS,

    //膝关节部分置换
    TKA_PARA_WIZARD_SET_TIPS,
    TKA_PARA_WIZARD_SELECT_FEMUR_PROXIMAL,
    TKA_PARA_WIZARD_SELECT_FEMUR_DISTAL,
    TKA_PARA_WIZARD_SELECT_FEMUR_FIBULA,
    TKA_PARA_WIZARD_SELECT_TIBIA_PROXIMAL,
    TKA_PARA_WIZARD_SELECT_TIBIA_DISTAL,

    TKA_PARA_FEMUR_CAVITY_LINE_SET_TIPS,
    TKA_PARA_FEMUR_CAVITY_LINE_SELECT_START_POINT,
    TKA_PARA_FEMUR_CAVITY_LINE_SELECT_END_POINT,

    TKA_PARA_FEMUR_AP_LINE_SET_TIPS,
    TKA_PARA_FEMUR_AP_LINE_SELECT_START_POINT,
    TKA_PARA_FEMUR_AP_LINE_SELECT_END_POINT,

    TKA_PARA_FEMUR_INSALL_AND_CONDYLE_LINE_SET_TIPS,
    TKA_PARA_FEMUR_INSALL_LINE_SELECT_START_POINT,
    TKA_PARA_FEMUR_INSALL_LINE_SELECT_END_POINT,
    TKA_PARA_FEMUR_CONDYLE_LINE_SELECT_START_POINT,
    TKA_PARA_FEMUR_CONDYLE_LINE_SELECT_END_POINT,

    TKA_PARA_FEMUR_DISTAL_LINE_SET_TIPS,
    TKA_PARA_FEMUR_DISTAL_LINE_SELECT_START_POINT,
    TKA_PARA_FEMUR_DISTAL_LINE_SELECT_END_POINT,

    TKA_PARA_TIBIA_AXIS_LINE_SET_TIPS,
    TKA_PARA_TIBIA_AXIS_LINE_SELECT_START_POINT,
    TKA_PARA_TIBIA_AXIS_LINE_SELECT_END_POINT,

    TKA_PARA_TIBIA_AP_AXIS_LINE_SET_TIPS,
    TKA_PARA_TIBIA_AP_AXIS_LINE_SELECT_START_POINT,
    TKA_PARA_TIBIA_AP_AXIS_LINE_SELECT_END_POINT,

    TKA_PARA_TIBIA_ML_LINE_SET_TIPS,
    TKA_PARA_TIBIA_ML_LINE_SELECT_START_POINT,
    TKA_PARA_TIBIA_ML_LINE_SELECT_END_POINT,

    TKA_PARA_TIBIA_MD_LINE_SET_TIPS,
    TKA_PARA_TIBIA_MD_LINE_SELECT_START_POINT,
    TKA_PARA_TIBIA_MD_LINE_SELECT_END_POINT,

    TKA_PARA_FEMUR_HEAD_SET_TIPS,
    TKA_PARA_FEMUR_HEAD_SELECT_FIRST_POINT,
    TKA_PARA_FEMUR_HEAD_SELECT_SECOND_POINT,

    TKA_PARA_FEMUR_GUIDE,
    TKA_PARA_TIBIA_GUIDE,

    //髋关节置换术前规划
    THRA_WIZARD_SET_TIPS,
    THRA_WIZARD_SELECT_HIP,
    THRA_WIZARD_SELECT_PELVICUM,

    THRA_SPHERE_SET_TIPS,
    THRA_SPHERE_SELECT_FIRST_POINT,
    THRA_SPHERE_SELECT_SECOND_POINT,
    THRA_SPHERE_SELECT_THIRD_POINT,
    THRA_SPHERE_SELECT_FOURTH_POINT,


    //髋关节全贴合
    THRA_TOTAL_WIZARD_SET_TIPS,

    THRA_TOTAL_SPLIT_SET_TIPS,
    THRA_TOATL_SPLIT_ADD_POINT,
    THRA_TOTAL_SPLIT_MODIFY_POINT,
    THRA_TOTAL_SPLIT_DELETE_POINT,
    THRA_TOTAL_SPLIT_MESH_SPLIT,

    THRA_TOTAL_THICK_SET_TIPS,
    THRA_TOTAL_THICK_MESH_THICKEN,

    THRA_TOTAL_GUIDE_SET_TIPS,
    THRA_TOTAL_GUIDE_ADD_CYLINDER_AND_BASE,
    THRA_TOTAL_GUIDE_GENERATE_GUIDE,




	//支具
	BRACE_WIZARD_SET_TIPS,

	//分割
	BRACE_SPLIT_SET_TIPS,

	BRACE_SPLIT_UP_SURFACE_CUT,
	BRACE_SPLIT_UP_SURFACE_CUT_ERROR,
	BRACE_SPLIT_SURFACE_CUT_UP_FINISH,

	BRACE_SPLIT_DOWN_SURFACE_CUT,
	BRACE_SPLIT_DOWN_SURFACE_CUT_ERROR,
	BRACE_SPLIT_SURFACE_CUT_DOWN_FINISH,

	//偏置
	BRACE_SPLIT_UP_SURFACE_OFFSET,
	BRACE_SPLIT_UP_SURFACE_OFFSET_ERROR,
	BRACE_SPLIT_UP_SURFACE_OFFSET_FINISH,

	BRACE_SPLIT_DOWN_SURFACE_OFFSET,
	BRACE_SPLIT_DOWN_SURFACE_OFFSET_ERROR,
	BRACE_SPLIT_DOWN_SURFACE_OFFSET_FINISH,

	//支具
	BRECE_HOLES_SET_TIPS,
	BRACE_HOLES_ONE,
	BRACE_HOLES_TWO,
	BRACE_HOLES_THREE,
	BRACE_HOLES_FOUR,
	BRACE_HOLES_FINISH,

	//模板
	BRACE_GUIDE_SET_TIPS,
	BRACE_GUIDE_GUIDE_FINISH,




    //股骨近端创伤
    TRAUMA_WIZARD_SET_TIPS,

    TRAUMA_DRAW_LINES_SET_TIPS,
    TRAUMA_DRAW_TRUNK_SELECT_START_POINT,
    TRAUMA_DRAW_TRUNK_SELECT_END_POINT,
    TRAUMA_DRAW_NECK_SELECT_START_POINT,
    TRAUMA_DRAW_NECK_SELECT_END_POINT,

    TRAUMA_DRAW_CUT_SET_TIPS,
    TRAUMA_DRAW_CUT_SELECT_START_POINT,
    TRAUMA_DRAW_CUT_SELECT_END_POINT,
    TRUAMA_DRAW_CUT_CONSTRAINT_VERTICAL,
    TRUAMA_DRAW_CUT_CUT_FEMUR,

    TRAUMA_LOCATE_POINTS_SET_TIPS,
    TRAUMA_LOCATE_POINTS_BONE,
    TRAUMA_LOCATE_POINTS_STEEL,

    //创伤交互
    TRAUMA_LOCATE_SET_TIPS,
    TRAUMA_LOCATE_SELECT_BONE,
    TRAUMA_LOCATE_SELECT_STEEL,
    TRAYMA_LOCATE_TRANSLATE,
    TRAUMA_LOCATE_ROTATE,
    TRAUMA_LOCATE_SHOW_ANGLE,
    TRAUMA_LOCATE_HIDE_ANGLE,


    //创伤部分贴合
    TRAUMA_GUIDE_SET_TIPS,
    TRAUMA_GUIDE_DRIVE_MODEL,
    TRAUMA_GUIDE_GENERATE_GUIDE,


    //矫形术前规划
    ORTHO_WIZARD_SET_TIPS,

    ORTHO_DISTAL_LINE_SET_TIPS,
    ORTHO_DISTAL_LINE_SELECT_START_POINT,
    ORTHO_DISTAL_LINE_SELECT_END_POINT,

    ORTHO_PROJECT_SET_TIPS,
    ORTHO_PROJECT_SELECT_LOCATE_POINT,
    ORTHO_PROJECT_SET_DIS,

    ORTHO_SAG_PLANE_SET_TIPS,
    ORTHO_SAG_PLANE_SELECT_LOCATE_POINT,
    ORTHO_SAG_PLANE_SET_ROTATE_ANGLE,
    ORTHO_SAG_PLANE_CUT_BONE,
    ORTHO_SAG_PLANE_CUT_BONE_BEFORE,

    ORTHO_COR_PLANE_SET_TIPS,
    ORTHO_COR_PLANE_SELECT_LOCATE_POINT,
    ORTHO_COR_PLANE_SET_ROTATE_ANGLE,
    ORTHO_COR_PLANE_CUT_BONE,
    ORTHO_COR_PLANE_CUT_BONE_BEFORE,

    ORTHO_LOCATE_STEEL_SET_TIPS,
    ORTHO_LOCATE_STEEL_LOAD_STEEL,
    ORTHO_LOCATE_STEEL_SELECT_NAIL,

    ORTHO_GUIDE_SET_TIPS,
    ORTHO_GUIDE_DRIVE_MODEL,
    ORTHO_GUIDE_UPDATE_GUIDE,

    ORTHO_TAG_SET_TIPS,
    ORTHO_TAG_SELECT_START_POINT_P1,
    ORTHO_TAG_SELECT_END_POINT_P2,
    ORTHO_TAG_SELECT_START_POINT_P3,
    ORTHO_TAG_SELECT_END_POINT_P4,
    ORTHO_TAG_CREATE_TAG_NAILS,


    //脊柱部分贴合
    SPINE_PARA_WIZARD_SET_TIPS,

    SPINE_PARA_LEFT_LINE_SET_TIPS,
    SPINE_PARA_LEFT_LINE_SELECT_START_POINT,
    SPINE_PARA_LEFT_LINE_SELECT_END_POINT,

    SPINE_PARA_RIGHT_LINE_SET_TIPS,
    SPINE_PARA_RIGHT_LINE_SELECT_START_POINT,
    SPINE_PARA_RIGHT_LINE_SELECT_END_POINT,

    SPINE_PARA_SUPPORT_POINT_SET_TIPS,
    SPINE_PARA_SUPPORT_POINT_SELECT_POINT,

    SPINE_PARA_GUIDE_SET_TIPS,
    SPINE_PARA_GUIDE_DRIVE_MODEL,
    SPINE_PARA_GUIDE_CREATE_GUIDE,


    //脊柱全贴合
    SPINE_TAOTL_WIZARD_SET_TIPS,

    SPINE_TAOTL_LEFT_LINE_SET_TIPS,
    SPINE_TAOTL_LEFT_LINE_SELECT_START_POINT,
    SPINE_TAOTL_LEFT_LINE_SELECT_END_POINT,

    SPINE_TAOTL_RIGHT_LINE_SET_TIPS,
    SPINE_TAOTL_RIGHT_LINE_SELECT_START_POINT,
    SPINE_TAOTL_RIGHT_LINE_SELECT_END_POINT,

    SPINE_TOTAL_SPLIT_SET_TIPS,
    SPINE_TOTAL_SPLIT_ADD_POINT,
    SPINE_TOTAL_SPLIT_MODIFY_POINT,
    SPINE_TOTAL_SPLIT_DELETE_POINT,
    SPINE_TOTAL_SPLIT_MESH,

    SPINE_TOTAL_GUIDE_SET_TIPS,
    SPINE_TOTAL_GUIDE_INCRE_MESH,
    SPINE_TOTAL_GUIDE_ADD_CYLINDER,
    SPINE_TOTAL_GUIDE_CREATE_GUIDE,

	//new status message by yh.cheng 2019-5-20
	STATUS_STRING_EMPRY,
	STATUS_STRING_PAGE_OM_CIRCLE_EDIT_PICK_POINT,

	STATUS_PICK_POINT,
	STATUS_PICK_SPHERE,
	STATUS_PICK_LINE,
	STATUS_PICK_LINE_SEGMENT,
	STATUS_PICK_PLANE,
	STATUS_PICK_PLANE1,
	STATUS_PICK_PLANE2,
	STATUS_PICK_POINT1,
	STATUS_PICK_POINT2,
	STATUS_PICK_NORMAL,
	STATUS_PICK_SPHERE_CENTER,

	STATUS_SKETCH_CREATE_PLANE,
	STATUS_SKETCH_LOCATE_PLANE,
	STATUS_CREATE_2D_POINT,
	STATUS_CREATE_2D_LINE,
	STATUS_CREATE_2D_Circle,
	STATUS_CREATE_2D_Arc,
	STATUS_CREATE_2D_SPINE,
	STATUS_CREATE_2D_SEGMENT,
	STATUS_CREATE_2D_RECT,
	STATUS_CONSTRAINT_ANGLE_BY_2D_LINE,
	STATUS_CONSTRAINT_RADIUS_BY_2D_CIRCLE,
	STATUS_CONSTRAINT_CENTER_BY_2D_CIRCLE,
	STATUS_FIX_POINT,
	STATUS_CONSTRAINT_LINE_LENGTH_BY_2D_LINE,
	STATUS_CONSTRAINT_LINE_DISTANCE_BY_2D_LINE,
	STATUS_CONSTRAINT_PARALLEL_BY_2D_LINE,
	STATUS_CONSTRAINT_VERTICAL_BY_2D_LINE,
	STATUS_CONSTRAINT_COLLINEAR_BY_2D_LINE,
	STATUS_CONSTRAINT_LENGTH_COMMON_BY_2D_LINE,
	STATUS_CONSTRAINT_POINT_COINCIDENCE,
	STATUS_CONSTRAINT_POINT_ON_lINE,
	STATUS_CONSTRAINT_HORIZONTAL,
	STATUS_CONSTRAINT_VERTICAL,
	STATUS_CONSTRAINT_TANGENT,
	STATUS_2D_MIRROR,
	STATUS_SKETCH_TRIM,
	STATUS_EXTEND,
	STATUS_FILLET,
	STATUS_DELETE_CONSTRINT,
	STATUS_2D_OFFSET,
	STATUS_2D_ROTATE,
	STATUS_2D_ARRAY,
	STATUS_ACTIVE_DISPLAY_MODEL,
	STATUS_ACTIVE_TRANSPARENT_SETTING,
	STATUS_CLICK_LEFT_BUTTON,
	STATUS_CLICK_LEFT_OFFSET,
	STATUS_CLICK_MIDDID_BUTTON_ROTATE,
	STATUS_ACTIVE_CONTINUE_ROTATE,
	STATUS_ACTIVE_AXIS_ROTATE,
	STATUS_ACTIVE_ADAPT_SCREEN,
	STATUS_ACTIVE_RECT_SELECT_ZOOM,
	STATUS_ACTIVE_ORTHOGONAL_PROJECTION,   
	STATUS_ACTIVE_PERSPECTIVE_PROJECTION,
	STATUS_ACTIVE_SINGLE_SELECT_OBJECT,
	STATUS_ACTIVE_RECT_SELECT_OBJECT_OPERATION,
	STATUS_ACTIVE_POLYGON_OBJECT_SELECT_OPERATION,
	STATUS_ACTIVE_SINGLE_SELECT_MESH_OPERATION,
	STATUS_ACTIVE_RECT_SELECT_MESH_OPERATION,
	STATUS_ACTIVE_CANCEL_SELECT_MESH_OPERATION,
	STATUS_ACTIVE_ARBITRARY_MARKUP_OPERATION,
	STATUS_ACTIVE_RECT_MARKUP_OPERATION,
	STATUS_ACTIVE_CLAEARUP_MARKUP_OPERATION,


}EXJSTATUSMESSAGE;


typedef enum XJMessageBoxType
{
    XJ_MESSAGE_YES,
    XJ_MESSAGE_YESNOCANCEL,
    XJ_MESSAGE_YESNO,


}EXJMessageBoxType;


typedef enum XJMBType
{
    XJ_MB_YES=0,
    XJ_MB_NO,
    XJ_MB_CANCEL,
}EXJMBType;

typedef enum XJPrivilege
{
	XJ_PRIVILEGE_A=0,
	XJ_PRIVILEGE_B,
	XJ_PRIVILEGE_C,
}EXJPrivilege;


typedef enum XJGuideOrthoPageIndex
{
	ORTHO_WIZARD_WORD = 0,
	ORTHO_PREPARE,
	ORTHO_INIT_LINE,
	ORTHO_INIT_PLANE,
	ORTHO_LEFT_VIEW_LINE,
	ORTHO_LEFT_VIEW_PLANE,
	ORTHO_LEFT_VIEW_ROTATE,
	ORTHO_FRONT_VIEW_LINE,
	ORTHO_FRONT_VIEW_PLANE,
	ORTHO_FRONT_VIEW_ROTATE,
	ORTHO_END = 10

}EXJGuideOrthoPageIndex;

typedef enum EXJLANGUAGE
{
	XJ_CHINESE,
	XJ_ENGLISH

}EXJLANGUAGE;


typedef enum TKAGUIDEPAGETYPE
{
	XJ_TKA_PROPERTY_PAGE_BEGIN = 0,
	XJ_TKA_PROPERTY_PAGE_WIZARD,
	XJ_TKA_PROPERTY_PAGE_FEMUR_CAVITY,
	XJ_TKA_PROPERTY_PAGE_WHITE_SIDE,
	XJ_TKA_PROPERTY_PAGE_FEMUR_INSALL_CONDYLE,
	XJ_TKA_PROPERTY_PAGE_FEMUR_DISTAL_LINE,
	XJ_TKA_PROPERTY_PAGE_TIBIA_AXIS,
	XJ_TKA_PROPERTY_PAGE_TIBIA_AP_AXIS,
	XJ_TKA_PROPERTY_PAGE_ML_LINE,
	XJ_TKA_PROPERTY_PAGE_MD_LINE,
	XJ_TKA_PROPERTY_PAGE_FEMUR_BALL,
	XJ_TKA_PROPERTY_PAGE_PARAMETER,
	XJ_TKA_PROPERTY_PAGE_END

}EXJTKAPropertyPage;

typedef enum XJBoneCutStatusType
{
	XJ_BONE_CUT_ORIGIN,
	XJ_BONE_CUT,
	XJ_BONE_CUT_COLOR,
	XJ_BONE_CUT_IMPLANT,
	XJ_BONE_CUT_TRANSPARENT,
	XJ_BONE_CUT_LIMB_LINE
} EXJBoneCutStatusType;


#define HTTP_NULL 1000
#define HTTP_UPLOAD 1001
#define HTTP_DOWNLOAD 1002


#define DB_UPLOAD_PATIENT_NULL 2000
#define DB_UPLOAD_PATIENT_ADD 2001
#define	DB_UPLOAD_PATIENT_MODIFY 2002
#define	DB_UPLOAD_PATIENT_DELETE 2003

#define DB_DOWNLOAD_PATIENT_NULL 2004
#define	DB_DOWNLOAD_PATIENT_ADD 2005
#define	DB_DOWNLOAD_PATIENT_MODIFY 2006
#define	DB_DOWNLOAD_PATIENT_DELETE 2007


#define DB_UPLOAD_DEVICE_NULL 2008
#define	DB_UPLOAD_DEVICE_ADD 2009
#define	DB_UPLOAD_DEVICE_MODIFY 2010
#define	DB_UPLOAD_DEVICE_DELETE 2011

#define DB_DOWNLOAD_DEVICE_NULL 2012
#define	DB_DOWNLOAD_DEVICE_ADD 2013
#define	DB_DOWNLOAD_DEVICE_MODIFY 2014
#define	DB_DOWNLOAD_DEVICE_DELETE 2015

#define DB_UPLOAD_DOCTOR_NULL 2016
#define	DB_UPLOAD_DOCTOR_ADD 2017
#define	DB_UPLOAD_DOCTOR_MODIFY 2018
#define	DB_UPLOAD_DOCTOR_DELETE 2020

#define DB_DOWNLOAD_DOCTOR_NULL 2021
#define	DB_DOWNLOAD_DOCTOR_ADD 2022
#define	DB_DOWNLOAD_DOCTOR_MODIFY 2023
#define	DB_DOWNLOAD_DOCTOR_DELETE 2024
	

#define NULL 0
#define XJPI 3.14159265358979324 //宏定义
#define XJ_TOL 1e-6
#define XJ_EDIT_TOL 1e-4
#define XJVIEW_MIN 0.1			
#define XJVIEW_MAX 10000
#define XJNUMBIT 13//  限定输入 数字长度小于13位；

struct indexTriplet {
	bool operator==(const indexTriplet& triplet) const {
		return ref[0]==triplet.ref[0] && ref[1]==triplet.ref[1] && ref[2]==triplet.ref[2];
	}
	bool operator<(const indexTriplet& triplet) const {
		if (ref[0]==triplet.ref[0]) {
			return ref[1]==triplet.ref[1] ? ref[2]<triplet.ref[2] : ref[1]<triplet.ref[1]; 
		}
		else {
			return ref[0]<triplet.ref[0]; 
		}
	}
	int ref[3];	//!< vertex indices
};




/*************DB*****************************************/

// /*定义病人表每项*/
#define	XJ_PATIENT_NUMBER			100001
#define	XJ_PATIENT_NAME				100002
#define	XJ_PATIENT_AGE				100003
#define	XJ_PATIENT_SEX				100004
#define	XJ_PATIENT_SIDE				100005
#define	XJ_PATIENT_OPERATION_TYPE	100006
#define XJ_PATIENT_OPERATION_TIME	100007
#define	XJ_PATIENT_DOCTOR_NUM		100008
#define	XJ_PATIENT_DEVICE_NUM1		100009
#define	XJ_PATIENT_DEVICE_NUM2		100010
#define	XJ_PATIENT_DEVICE_NUM3		100011
#define	XJ_PATIENT_DEVICE_NUM4		100012
#define	XJ_PATIENT_FILE_PATH		100013
#define XJ_PATIENT_CONFIRM          100014

// /*定义设备表每项*/
#define	XJ_DEVICE_NUMBER			100001
#define	XJ_DEVICE_COMPANY			100002
#define	XJ_DEVICE_BRAND				100003
#define	XJ_DEVICE_TYPE				100004
#define	XJ_DEVICE_BODYPART			100005
#define	XJ_DEVICE_DEVICE_SIDE		100006
#define XJ_DEVICE_DEVICE_SIZE		100007
#define XJ_DEVICE_FILE_PATH		    100008
// /*定义医生表每项*/
#define	XJ_DOCTOR_NUMBER			100001
#define	XJ_DOCTOR_NAME				100002
#define	XJ_DOCTOR_AGE			    100003
#define	XJ_DOCTOR_SEX			    100004
#define	XJ_DOCTOR_DEPARTMENT		100005
#define	XJ_DOCTOR_JOB		        100006
#define XJ_DOCTOR_PATIENT_NUM	    100007
// /*定义注册用户表每项*/
#define	XJ_ROLE_ACCOUNT				100001
#define	XJ_ROLE_PASSWORD			100002
#define	XJ_ROLE_PRIVILEGE			100003



#define	XJ_Tabl_DEPARTMENT		100005
#define	XJ_DOCTOR_JOB		        100006
#define XJ_DOCTOR_PATIENT_NUM	    100007
#define XJ_DOCTOR_HOSPITAL          100008

#define  XJ_TABLE_OM_KEY_BASE  100000 

#define XJ_ERROR_FILE_UNOPEN 19900
#define XJ_ERROR_FILE_VERSION_DEGREE 199901
#define XJ_ERROR_FILE_VERSION_INFO 199902
#define XJ_ERROR_FILE_FORMAT 199903
#define XJ_ERROR_FILE_PATH_EMPTY 199904

#define XJ_ERROR_POINT   20000
#define XJ_ERROR_LINE    20000+1
#define XJ_ERROR_CIRCLE  20000+2
#define XJ_ERROR_PLANE   20000+3
#define XJ_ERROR_BOX     20000+4
#define XJ_ERROR_SPHERE  20000+5
#define XJ_ERROR_CYLINDER  20000+6
#define XJ_ERROR_POLYLINE  20000+7
#define XJ_ERROR_CURVESPLINE  20000+8
#define XJ_ERROR_CONE  20000+9
#define XJ_ERROR_MESH  20000+10
#define XJ_ERROR_MESH_CONE  20000+11
#define XJ_ERROR_MESH_CYLINDER  20000+12
#define XJ_ERROR_MESH_PLANE  20000+13
#define XJ_ERROR_MESH_BOX  20000+14
#define XJ_ERROR_MESH_SPHERE  20000+15
#define XJ_ERROR_MESH_CIRCLE  20000+16
#define XJ_ERROR_MESH_MESH_CIRCLE  20000+17
#define XJ_ERROR_MEASURE_DIS  20000+18
#define XJ_ERROR_MEASURE_ANG  20000+19

#define XJ_ERROR_TKA_SURGICAL_GROUP  20000+30
#define XJ_ERROR_TKA_SURG_CLIENT_GRP  20000+31
#define XJ_ERROR_TKA_FEMUR_GROUP  20000+32
#define XJ_ERROR_TKA_TIBIA_GROUP  20000+33
#define XJ_ERROR_TKA_OSTEOTOME_GROUP  20000+34


#define XJ_ERROR_THA_SURGICAL_GROUP  20000+35
#define XJ_ERROR_THA_SURG_CLIENT_GRP  20000+36
#define XJ_ERROR_THA_GROUP  20000+37

#define XJ_ERROR_TKA_TRADITIONAL_GROUP  20000+38
#define XJ_ERROR_TKA_WWB_GROUP  20000+39

#define XJ_ERROR_THA_TRANDITIONAL_GROUP  20000+40
#define XJ_ERROR_FEMUR_NEAR_SURG_GROUP  20000+41
#define XJ_ERROR_FEMUR_NEAR_GUIDE_GROUP  20000+42
#define XJ_ERROR_FEMUR_FAR_GUIDE_GROUP  20000+43
#define XJ_ERROR_FEMUR_FAR_SURG_GROUP  20000+44
#define XJ_ERROR_BRACE_GROUP  20000+45
#define XJ_ERROR_SPINE_PART_ATTACH_GROUP  20000+46
#define XJ_ERROR_SPINE_TOTAL_ATTACH_GROUP  20000+47

#define XJ_ERROR_SKETCH_GROUP 20000+48

#define XJ_ERROR_2D_POINT  20000+49
#define XJ_ERROR_2D_PLANE  20000+50
#define XJ_ERROR_2D_LINE   20000+51
#define XJ_ERROR_2D_CIRCLE 20000+52
#define XJ_ERROR_2D_ELLIPSE 20000+53
#define XJ_ERROR_2D_BSPLINE 20000+54
#define XJ_ERROR_2D_POLYLINE 20000+55

#define XJ_NONE 0
#define XJ_EXISTPOINT 1
#define XJ_ENDPOINT (1<<1)
#define XJ_MIDPOINT (1<<2)
#define XJ_CENPOINT (1<<3)
#define XJ_POINTONCURVE (1<<4)
#define XJ_ALLPOINT (XJ_EXISTPOINT|XJ_ENDPOINT|XJ_MIDPOINT|XJ_CENPOINT|XJ_POINTONCURVE)
/*定义数据库数据结构体*/
 typedef struct db_xjpatient
 {
	 db_xjpatient()
	 {
		 db_number = "";         //病人编号
		 db_name = "";           //姓名
		 db_age = "";		     //年龄
		 db_sex = "";			 //性别
		 db_side ="";		     //患侧
		 db_type = "";			 //手术类型
		 db_date = "";			 //手术日期
		 db_doctornumber = "";	 //医生编号
		 db_devicenumber1 = "";	     //设备编号
		 db_devicenumber2 = "";	     //设备编号
		 db_devicenumber3 = "";	     //设备编号
		 db_devicenumber4 = "";	     //设备编号
		 db_path = "";			  //路径
		 db_confirm = "";
	 }

	 ~db_xjpatient()
	 {
		 db_number = "";         //病人编号
		 db_name = "";           //姓名
		 db_age = "";		     //年龄
		 db_sex = "";			 //性别
		 db_side ="";		     //患侧
		 db_type = "";			 //手术类型
		 db_date = "";			 //手术日期
		 db_doctornumber = "";	 //医生编号
		 db_devicenumber1 = "";	     //设备编号
		 db_devicenumber2 = "";	     //设备编号
		 db_devicenumber3 = "";	     //设备编号
		 db_devicenumber4 = "";	     //设备编号
		 db_path = "";			  //路径
		 db_confirm = "";
	 }

 	std::string    db_number;         //病人编号
 	std::string    db_name;           //姓名
	std::string    db_age;		     //年龄
 	std::string    db_sex;			 //性别
	std::string    db_side;		     //患侧
	std::string    db_type;			 //手术类型
 	std::string    db_date;			 //手术日期
	std::string    db_doctornumber;	 //医生编号
 	std::string    db_devicenumber1;	 //设备编号
	std::string    db_devicenumber2;	 //设备编号
	std::string    db_devicenumber3;	 //设备编号
	std::string    db_devicenumber4;	 //设备编号
	std::string    db_path;			  //路径
	std::string    db_confirm;
 }structPatientRecode;

 /*定义数据库设备结构体*/
 typedef struct db_xjdevice
 {
	 db_xjdevice()
	 {
		 db_number = "";         //设备编号
		 db_company = "";        //厂商
		 db_brand ="";		     //品牌
		 db_type ="";			 //设备种类
		 db_bodypart="";		 //用于身体部位
		 db_side="";			 //左右
		 db_size="";			 //大小尺寸
		 db_path="";		 //路径
	 }

	 ~db_xjdevice()
	 {
		 db_number = "";         //设备编号
		 db_company = "";        //厂商
		 db_brand ="";		     //品牌
		 db_type ="";			 //设备种类
		 db_bodypart="";		 //用于身体部位
		 db_side="";			 //左右
		 db_size="";			 //大小尺寸
		 db_path="";		 //路径
	 }

	 std::string    db_number;         //设备编号
	 std::string    db_company;        //厂商
	 std::string    db_brand;		  //品牌
	 std::string    db_type;			  //设备种类
	 std::string    db_bodypart;		  //用于身体部位
	 std::string    db_side;			  //左右
	 std::string    db_size;			  //大小尺寸
	 std::string    db_path;			  //路径

 }structDeviceRecode;

 /*定义数据库医生信息结构体*/
 typedef struct db_xjdoctor
 {

	 db_xjdoctor()
	 {
		 db_number = "";         //设备编号
		 db_name = "";        //厂商
		 db_age ="";		     //品牌
		 db_sex ="";			 //设备种类
		 db_department="";		 //用于身体部位
		 db_job="";			 //左右
		 db_hospital="";         //医院
	 }

	 ~db_xjdoctor()
	 {
		 db_number = "";         //设备编号
		 db_name = "";           //厂商
		 db_age ="";		     //品牌
		 db_sex ="";			 //设备种类
		 db_department="";		 //用于身体部位
		 db_job="";			     //左右
		 db_hospital="";         //医院
	 }

	 std::string    db_number;         //医生编号
	 std::string    db_name;           //姓名
	 std::string    db_age;		      //年龄
	 std::string    db_sex;			  //性别
	 std::string    db_department;	  //部门
	 std::string    db_job;			  //职称
	 std::string    db_hospital;      //医院

 }structDoctorRecode;

 /*定义数据库医生信息结构体*/
 typedef struct db_xjrole
 {

	 db_xjrole()
	 {
		 db_account = "";
		 db_password = ""; 
		 db_privilege ="";
	 }

	 ~db_xjrole()
	 {
		 db_account = ""; 
		 db_password = "";       
		 db_privilege ="";		  
	 }

	 std::string db_account;   //账号名
	 std::string db_password;  //密码  
	 std::string db_privilege; //用户权限等级
 }structRoleRecode;

 typedef struct FuncLicence
 {
	 FuncLicence()
	 {
		 bFuncBrace=false;//支具功能
		 bFuncOrthoTotal=false;
		 bFuncOrthoPart=false;
		 bFuncSpineTotal=false;
		 bFuncSpinePart=false;
		 bFuncTHATotal=false;
		 bFuncTHAPart=false;
		 bFuncTKATotal=false;//TKATotal功能
		 bFuncTKAPart=false;//TKAPart功能
		 bFuncTraumaTotal=false;
		 bFuncTraumaPart=false;
		 bFuncJaw = false;
		 bFuncFracReduction = false;
		 bFuncFileOper=false;
		 bFuncTKAImpDesign = false;
		 bFuncTumourGuide = false;
		 bFuncAnkleGuide = false;
		 bFuncHTOGuide = false;
		 bFuncCoreGuide = false;

		 bModuleGuideTka = false; 
		 bModuleGuideTha = false;
		 bModuleGuideTrauma = false;
		 bModuleGuideBrace = false;
		 bModuleGuideSpine = false;

		 bDisplaySoftdata = false;
		 bDisplayHarddata = false;
		 bDisplayBraceData = false;

		 bDisplayGuideTka = false;
		 bDisplayGuideTha = false;
		 bDisplayGuideTrauma = false;
		 bDisplayGuideSpine = false;
		 bDisplayGuideBrace = false;
	 }

	 ~FuncLicence()
	 {
	 }

	 bool bFuncBrace;//支具功能
	 bool bFuncOrthoTotal;
	 bool bFuncOrthoPart;
	 bool bFuncSpineTotal;
	 bool bFuncSpinePart;
	 bool bFuncTHATotal;
	 bool bFuncTHAPart;
	 bool bFuncTKATotal;//TKATotal功能
	 bool bFuncTKAPart;//TKAPart功能
	 bool bFuncTraumaTotal;
	 bool bFuncTraumaPart;
	 bool bFuncJaw;
	 bool bFuncFracReduction;
	 bool bFuncFileOper;
	 bool bFuncTKAImpDesign;
	 bool bFuncTumourGuide;
	 bool bFuncAnkleGuide;
	 bool bFuncHTOGuide;
	 bool bFuncCoreGuide;

	 //guide display
	 bool bModuleGuideTka;
	 bool bModuleGuideTha;
	 bool bModuleGuideTrauma;
	 bool bModuleGuideBrace;
	 bool bModuleGuideSpine;

	 //model display
	 bool bDisplaySoftdata;
	 bool bDisplayHarddata;
	 bool bDisplayBraceData;

	 bool bDisplayGuideTka;
	 bool bDisplayGuideTha;
	 bool bDisplayGuideTrauma;
	 bool bDisplayGuideSpine;
	 bool bDisplayGuideBrace;

 }structFuncLicence;


 typedef struct FuncMenu
 {
	 FuncMenu()
	 {
		 bHome=false;//支具功能
		 bAlign=false;
		 bPlan=false;
		 bAnalyze=false;
		 bMeasure=false;
		 bMark=false;
		 bFix=false;
		 bView=false;//TKATotal功能
		 bSketch = false;
		 bDesign=false;//TKAPart功能
		 bTooling=false;
		 bLightweights=false;
		 bPlanDisyplay=false;
		 bModuleDisplay=false;
	 }

	 ~FuncMenu()
	 {
		 bHome=false;//支具功能
		 bAlign=false;
		 bPlan=false;
		 bAnalyze=false;
		 bMeasure=false;
		 bMark=false;
		 bFix=false;
		 bView=false;//TKATotal功能
		 bSketch = false;
		 bDesign=false;//TKAPart功能
		 bTooling=false;
		 bLightweights=false;
		 bPlanDisyplay=false;
		 bModuleDisplay=false;
	 }

	 bool bHome;//支具功能
	 bool bAlign;
	 bool bPlan;
	 bool bAnalyze;
	 bool bMeasure;
	 bool bMark;
	 bool bFix;
	 bool bView;
	 bool bSketch;
	 bool bDesign;
	 bool bTooling;
	 bool bLightweights;
	 bool bPlanDisyplay;//TKAPart功能
	 bool bModuleDisplay;

 }structFuncMenu;


 typedef enum XJTableType
 {
	 XJ_TABLE_DOCTOR = 0,
	 XJ_TABLE_PATIENT,
	 XJ_TABLE_DEVICE,
	 XJ_TABLE_ROLE
 } EXJTableType;

 typedef struct XJdatabaseInfo
 {
	 string sHostAddress;//主机地址
	 string sUserName;//用户名
	 string sPassword;//密码
	 string sDataBaseName;//数据库名
	 string sDatabasePort;//主机端口
 } XJDBInfo;



typedef struct XJUploadInfo
{
	XJUploadInfo()
	{
		//remoteDomainAddress = "192.168.1.252";
		remoteDomainAddress = "127.0.0.1";
	    nRemotePortNumber = 8888;
		remoteFilePath = "";

		localType ="";
		localCharset = "gb2312";
		localFilePath = "";
	}

	string remoteDomainAddress;
	int nRemotePortNumber;
	string remoteFilePath;

	string localType;
	string localCharset;
	string localFilePath;

}XJUploadInformation;


typedef struct XJDownloadInfo
{
	XJDownloadInfo()
	{
		remoteDomainAddress = "127.0.0.1";
		nRemotePortNumber = 8888;
		remoteHttpHeadInfo = "";

		localType ="";
		localCharset = "gb2312";
		localFilePath = "";
	}

	string remoteDomainAddress;
	int nRemotePortNumber;
	string remoteHttpHeadInfo;

	string localType;
	string localCharset;
	string localFilePath;

}XJDownloadInformation;


struct VecArrayName
{
	osg::ref_ptr<osg::Vec3Array> array;
	QString name;
};


struct InputData
{
	Point3D point;
	float radius;
	bool isRight;   // 是否为右边加工
};

struct JiaGongInfo
{
	float start;
	float end;
	int iWan;
};

struct AnimationInfo
{
	double fStartTime;   // 开始时间
	double fEndTime;     // 结束时间
	Point3D translate;  // 平移总量
	float radius;        // 当前半径
	float rotateX;       // 旋转角度(弧度)
	float rotateY;       // 旋转角度(弧度)
	float rotateZ;
	bool back;
	int iAxis;//1x,2y,3z
	int iAnimationType;  // 1平移，2 旋转
	osg::Vec3 center3d;
	int fdir;            //0：向左弯，右边加工，1向右弯，左边加工
	int pipeSeg;           // 第几段加工的管子
	int pipeSegTw;         // 第几段推弯加工的管子
	bool isTW;
	bool isZhujiaZ;         // 判断是否是主夹相对于机头Z方向的运动
    bool isZhujiaX;         // 判断是否是朱家相对于机台X方向的运动
	bool isZhujiaR;         // 判断是否主夹相对机头机头滑动
	osg::ref_ptr<osg::Geode> group;
	PIPE* pipe;
    osg::Vec3 absolutePos;

	double fColorStartTime;   // 开始时间
	double fColorEndTime;     // 结束时间

    double dDeltaX;
    double dDeltaZ;

	std::string strProcess;

	AnimationInfo()
	{
		fStartTime = fEndTime = -1;
		fColorStartTime = fColorEndTime = -1;
		translate = Point3D(0.0f,0.0f,0.0f);
		radius = 0;
		rotateX = rotateY = rotateZ= 0;
		back = true;
		center3d = osg::Vec3(0.0f,0.0f,0.0f);
		fdir = -1.0;
		pipeSeg = -1;
		group = NULL;
		pipe = NULL;
		iAxis = -1;
		iAnimationType =-1;
		isTW = false;
		isZhujiaZ = false;
        isZhujiaX = false;
		isZhujiaR = false;
		strProcess = "";
        absolutePos = osg::Vec3();

        dDeltaX = 0.0;  
        dDeltaZ = 0.0;
	}
};

//#define MODEL_CONFFIG_FILE   "C:/ModelConfig/模型配置文件.ini"
#define MODEL_CONFFIG_FILE   "/files/ModelConfig/ModelConfig.ini"

#define MAX_STR_NUM 16
#define MAX_STR_LENGTH 256
#define MACHINE              "MACHINE"
#define CONFIG               "CONFIG"

#define MODEL_CHETOU     "MODEL_CHETOU"
#define MODEL_DAOLUN1     "MODEL_DAOLUN1"
#define MODEL_DAOLUN2     "MODEL_DAOLUN2"
#define MODEL_JITOUZHAOKE "MODEL_JITOUZHAOKE"
#define MODEL_ZHULUN     "MODEL_ZHULUN"

#define R_AUTOLOAD_MODEL     "R_AUTOLOAD_MODEL"
#define R_MODEL_TYPE         "R_MODEL_TYPE"
#define MODEL_JISHEN         "MODEL_JISHEN"
#define MODEL_DIMIAN         "MODEL_DIMIAN"
#define MODEL_XIAOCHE        "MODEL_XIAOCHE"
#define MODEL_ZHUALIAOMO     "MODEL_ZHUALIAOMO"
#define MODEL_DAOJIA2_YOFF   "MODEL_DAOJIA2_YOFF"
#define MODEL_DAOJIA1        "MODEL_DAOJIA1"
#define MODEL_DAOJIA1_1      "MODEL_DAOJIA1_1"
#define MODEL_DAOJIA2        "MODEL_DAOJIA2"
#define MODEL_DAOJIA2_1      "MODEL_DAOJIA2_1"
#define MODEL_ZHUJIA1        "MODEL_ZHUJIA1"
#define MODEL_ZHUJIA2        "MODEL_ZHUJIA2"
#define MODEL_JITOUDIZUO     "MODEL_JITOUDIZUO"
#define MODEL_JITOUMOZUO     "MODEL_JITOUMOZUO"
#define MODEL_JITOUWANGUAN1  "MODEL_JITOUWANGUAN1"
#define MODEL_JITOUWANGUAN2  "MODEL_JITOUWANGUAN2"
#define MODEL_JITOUWANGUAN3  "MODEL_JITOUWANGUAN3"
#define MODEL_ZHONGXINMO     "MODEL_ZHONGXINMO"
#define MODEL_WANQUMO1       "MODEL_WANQUMO1"
#define MODEL_WANQUMO2       "MODEL_WANQUMO2"
#define MODEL_DJ1_1          "MODEL_DJ1_1"
#define MODEL_DJ2_1          "MODEL_DJ2_1"
#define MODEL_WANQUMO_1_1    "MODEL_WANQUMO_1_1"
#define MODEL_FUTUI_1        "MODEL_FUTUI_1"
#define MODEL_FUTUI_1_1      "MODEL_FUTUI_1_1"
#define MODEL_ZUOFUTUI       "MODEL_ZUOFUTUI"
#define MODEL_YOUFUTUI       "MODEL_YOUFUTUI"
#define MODEL_WANBI			 "MODEL_WANBI"
#define MODEL_WANQUMO1_1	 "MODEL_WANQUMO1_1"

#define MODEL_ZHUJIA_Z_OFFSET "MODEL_ZHUJIA_Z_OFFSET"
#define MODEL_ZHUJIA_Y_OFFSET "MODEL_ZHUJIA_Y_OFFSET"
#define MODEL_LunMO_R    "MODEL_LunMO_R"
#define MODEL_DAOJIAHEIGHT "MODEL_DAOJIAHEIGHT"
#define MODEL_ZHONGXINMOHEIGHT "MODEL_ZHONGXINMOHEIGHT"

#define MODEL_ZHUJIA1_LOW       "MODEL_ZHUJIA1_LOW"
#define MODEL_DAOJIA1_LOW "MODEL_DAOJIA1_LOW"
#define MODEL_LMAOCAO_R "MODEL_LMAOCAO_R"
#define ZORE_MOVE "ZORE_MOVE"
#define ZHUJIA2_CHANGEMOVE "ZHUJIA2_CHANGEMOVE"
#define PARAM_PATH "PARAM_PATH" 
#define TWLC_PATH "TWLC_PATH" 
#define STL_PATH "STL_PATH" 

#define MCAH_TYPE "MACH_TYPE"
#define ALLFILE_PATH "ALLFILE_PATH" 

#define ZORE_ZHUA_ANGLE "ZORE_ZHUA_ANGLE" 

#define ZORE_B_ANGLE "ZORE_B_ANGLE"
#define ZHUALIAOMO_ZhICHENGDIS "ZHUALIAOMO_ZhICHENGDIS" 
#define JITOUMOZUO_ZORE_MOVE "JITOUMOZUO_ZORE_MOVE" 
#define JITOUDIZUO_ZORE_MOVE "JITOUDIZUO_ZORE_MOVE" 
#define ZHULUN_AOCAO_R "ZHULUN_AOCAO_R" 
#define JITOUDIZUO_LOWER_LEN "JITOUDIZUO_LOWER_LEN"
#define JITOUWANGUAN1_ZORE_ANGLE "JITOUWANGUAN1_ZORE_ANGLE"
#define JITOUWANGUAN2_ZORE_ANGLE "JITOUWANGUAN2_ZORE_ANGLE"
#define DAOLUN1_JIAJIN_ANGLE "DAOLUN1_JIAJIN_ANGLE"
#define DAOLUN2_JIAJIN_ANGLE "DAOLUN2_JIAJIN_ANGLE"
#define ZHULUN_R "ZHULUN_R"

#define LONG_PIPE_CENTER_X "LONG_PIPE_CENTER_X"
#define LONG_PIPE_CENTER_Y "LONG_PIPE_CENTER_Y"
#define LONG_PIPE_CENTER_Z "LONG_PIPE_CENTER_Z"

#define LONG_PIPE_NORMAL_X "LONG_PIPE_NORMAL_X"
#define LONG_PIPE_NORMAL_Y "LONG_PIPE_NORMAL_Y"
#define LONG_PIPE_NORMAL_Z "LONG_PIPE_NORMAL_Z"

#define PIPE_CENTER_X_1 "PIPE_CENTER_X_1"
#define PIPE_CENTER_Y_1 "PIPE_CENTER_Y_1"
#define PIPE_CENTER_Z_1 "PIPE_CENTER_Z_1"

#define PIPE_CENTER_X_1 "PIPE_CENTER_X_1"
#define PIPE_CENTER_Y_1 "PIPE_CENTER_Y_1"
#define PIPE_CENTER_Z_1 "PIPE_CENTER_Z_1"

#define PIPE_CENTER_X_2 "PIPE_CENTER_X_2"
#define PIPE_CENTER_Y_2 "PIPE_CENTER_Y_2"
#define PIPE_CENTER_Z_2 "PIPE_CENTER_Z_2"

#define PIPE_CENTER_X_3 "PIPE_CENTER_X_3"
#define PIPE_CENTER_Y_3 "PIPE_CENTER_Y_3"
#define PIPE_CENTER_Z_3 "PIPE_CENTER_Z_3"

#define PIPE_CENTER_X_4 "PIPE_CENTER_X_4"
#define PIPE_CENTER_Y_4 "PIPE_CENTER_Y_4"
#define PIPE_CENTER_Z_4 "PIPE_CENTER_Z_4"

#define PIPE_CENTER_X_5 "PIPE_CENTER_X_5"
#define PIPE_CENTER_Y_5 "PIPE_CENTER_Y_5"
#define PIPE_CENTER_Z_5 "PIPE_CENTER_Z_5"

#define PIPE_CENTER_X_6 "PIPE_CENTER_X_6"
#define PIPE_CENTER_Y_6 "PIPE_CENTER_Y_6"
#define PIPE_CENTER_Z_6 "PIPE_CENTER_Z_6"

#define PIPE_CENTER_X_7 "PIPE_CENTER_X_7"
#define PIPE_CENTER_Y_7 "PIPE_CENTER_Y_7"
#define PIPE_CENTER_Z_7 "PIPE_CENTER_Z_7"

#define PIPE_CENTER_X_8 "PIPE_CENTER_X_8"
#define PIPE_CENTER_Y_8 "PIPE_CENTER_Y_8"
#define PIPE_CENTER_Z_8 "PIPE_CENTER_Z_8"

#define PIPE_CENTER_X_9 "PIPE_CENTER_X_9"
#define PIPE_CENTER_Y_9 "PIPE_CENTER_Y_9"
#define PIPE_CENTER_Z_9 "PIPE_CENTER_Z_9"

#define PIPE_CENTER_X_10 "PIPE_CENTER_X_10"
#define PIPE_CENTER_Y_10 "PIPE_CENTER_Y_10"
#define PIPE_CENTER_Z_10 "PIPE_CENTER_Z_10"

// 机头中心
#define HEADER_CENTER_X_1 "HEADER_CENTER_X_1"
#define HEADER_CENTER_Y_1 "HEADER_CENTER_Y_1"
#define HEADER_CENTER_Z_1 "HEADER_CENTER_Z_1"

#define HEADER_CENTER_X_2 "HEADER_CENTER_X_2"
#define HEADER_CENTER_Y_2 "HEADER_CENTER_Y_2"
#define HEADER_CENTER_Z_2 "HEADER_CENTER_Z_2"

#define HEADER_CENTER_X_3 "HEADER_CENTER_X_3"
#define HEADER_CENTER_Y_3 "HEADER_CENTER_Y_3"
#define HEADER_CENTER_Z_3 "HEADER_CENTER_Z_3"

#define HEADER_CENTER_X_4 "HEADER_CENTER_X_4"
#define HEADER_CENTER_Y_4 "HEADER_CENTER_Y_4"
#define HEADER_CENTER_Z_4 "HEADER_CENTER_Z_4"

#define HEADER_CENTER_X_5 "HEADER_CENTER_X_5"
#define HEADER_CENTER_Y_5 "HEADER_CENTER_Y_5"
#define HEADER_CENTER_Z_5 "HEADER_CENTER_Z_5"

#define HEADER_CENTER_X_6 "HEADER_CENTER_X_6"
#define HEADER_CENTER_Y_6 "HEADER_CENTER_Y_6"
#define HEADER_CENTER_Z_6 "HEADER_CENTER_Z_6"

#define HEADER_CENTER_X_7 "HEADER_CENTER_X_7"
#define HEADER_CENTER_Y_7 "HEADER_CENTER_Y_7"
#define HEADER_CENTER_Z_7 "HEADER_CENTER_Z_7"

#define HEADER_CENTER_X_8 "HEADER_CENTER_X_8"
#define HEADER_CENTER_Y_8 "HEADER_CENTER_Y_8"
#define HEADER_CENTER_Z_8 "HEADER_CENTER_Z_8"

#define HEADER_CENTER_X_9 "HEADER_CENTER_X_9"
#define HEADER_CENTER_Y_9 "HEADER_CENTER_Y_9"
#define HEADER_CENTER_Z_9 "HEADER_CENTER_Z_9"

#define HEADER_CENTER_X_10 "HEADER_CENTER_X_10"
#define HEADER_CENTER_Y_10 "HEADER_CENTER_Y_10"
#define HEADER_CENTER_Z_10 "HEADER_CENTER_Z_10"

#define PIPE_NORMAL_X_1 "PIPE_NORMAL_X_1"
#define PIPE_NORMAL_Y_1 "PIPE_NORMAL_Y_1"
#define PIPE_NORMAL_Z_1 "PIPE_NORMAL_Z_1"

#define PIPE_NORMAL_X_2 "PIPE_NORMAL_X_2"
#define PIPE_NORMAL_Y_2 "PIPE_NORMAL_Y_2"
#define PIPE_NORMAL_Z_2 "PIPE_NORMAL_Z_2"

#define PIPE_NORMAL_X_3 "PIPE_NORMAL_X_3"
#define PIPE_NORMAL_Y_3 "PIPE_NORMAL_Y_3"
#define PIPE_NORMAL_Z_3 "PIPE_NORMAL_Z_3"

#define PIPE_NORMAL_X_4 "PIPE_NORMAL_X_4"
#define PIPE_NORMAL_Y_4 "PIPE_NORMAL_Y_4"
#define PIPE_NORMAL_Z_4 "PIPE_NORMAL_Z_4"

#define PIPE_NORMAL_X_5 "PIPE_NORMAL_X_5"
#define PIPE_NORMAL_Y_5 "PIPE_NORMAL_Y_5"
#define PIPE_NORMAL_Z_5 "PIPE_NORMAL_Z_5"

#define PIPE_NORMAL_X_6 "PIPE_NORMAL_X_6"
#define PIPE_NORMAL_Y_6 "PIPE_NORMAL_Y_6"
#define PIPE_NORMAL_Z_6 "PIPE_NORMAL_Z_6"

#define PIPE_NORMAL_X_7 "PIPE_NORMAL_X_7"
#define PIPE_NORMAL_Y_7 "PIPE_NORMAL_Y_7"
#define PIPE_NORMAL_Z_7 "PIPE_NORMAL_Z_7"

#define PIPE_NORMAL_X_8 "PIPE_NORMAL_X_8"
#define PIPE_NORMAL_Y_8 "PIPE_NORMAL_Y_8"
#define PIPE_NORMAL_Z_8 "PIPE_NORMAL_Z_8"

#define PIPE_NORMAL_X_9 "PIPE_NORMAL_X_9"
#define PIPE_NORMAL_Y_9 "PIPE_NORMAL_Y_9"
#define PIPE_NORMAL_Z_9 "PIPE_NORMAL_Z_9"

#define PIPE_NORMAL_X_10 "PIPE_NORMAL_X_10"
#define PIPE_NORMAL_Y_10 "PIPE_NORMAL_Y_10"
#define PIPE_NORMAL_Z_10 "PIPE_NORMAL_Z_10"

#define PIPE_ROTATE_ANGLE_1   "PIPE_ROTATE_ANGLE_1"
#define PIPE_ROTATE_ANGLE_2   "PIPE_ROTATE_ANGLE_2"
#define PIPE_ROTATE_ANGLE_3   "PIPE_ROTATE_ANGLE_3"
#define PIPE_ROTATE_ANGLE_4   "PIPE_ROTATE_ANGLE_4"
#define PIPE_ROTATE_ANGLE_5   "PIPE_ROTATE_ANGLE_5"
#define PIPE_ROTATE_ANGLE_6   "PIPE_ROTATE_ANGLE_6"
#define PIPE_ROTATE_ANGLE_7   "PIPE_ROTATE_ANGLE_7"
#define PIPE_ROTATE_ANGLE_8   "PIPE_ROTATE_ANGLE_8"
#define PIPE_ROTATE_ANGLE_9   "PIPE_ROTATE_ANGLE_9"
#define PIPE_ROTATE_ANGLE_10  "PIPE_ROTATE_ANGLE_10"

#define PLANE_CENTER_X_1 "PLANE_CENTER_X_1"
#define PLANE_CENTER_Y_1 "PLANE_CENTER_Y_1"
#define PLANE_CENTER_Z_1 "PLANE_CENTER_Z_1"

#define PLANE_CENTER_X_2 "PLANE_CENTER_X_2"
#define PLANE_CENTER_Y_2 "PLANE_CENTER_Y_2"
#define PLANE_CENTER_Z_2 "PLANE_CENTER_Z_2"

#define PLANE_CENTER_X_3 "PLANE_CENTER_X_3"
#define PLANE_CENTER_Y_3 "PLANE_CENTER_Y_3"
#define PLANE_CENTER_Z_3 "PLANE_CENTER_Z_3"

#define PLANE_CENTER_X_4 "PLANE_CENTER_X_4"
#define PLANE_CENTER_Y_4 "PLANE_CENTER_Y_4"
#define PLANE_CENTER_Z_4 "PLANE_CENTER_Z_4"

#define PLANE_CENTER_X_5 "PLANE_CENTER_X_5"
#define PLANE_CENTER_Y_5 "PLANE_CENTER_Y_5"
#define PLANE_CENTER_Z_5 "PLANE_CENTER_Z_5"

#define PLANE_CENTER_X_6 "PLANE_CENTER_X_6"
#define PLANE_CENTER_Y_6 "PLANE_CENTER_Y_6"
#define PLANE_CENTER_Z_6 "PLANE_CENTER_Z_6"

#define PLANE_CENTER_X_7 "PLANE_CENTER_X_7"
#define PLANE_CENTER_Y_7 "PLANE_CENTER_Y_7"
#define PLANE_CENTER_Z_7 "PLANE_CENTER_Z_7"

#define PLANE_CENTER_X_8 "PLANE_CENTER_X_8"
#define PLANE_CENTER_Y_8 "PLANE_CENTER_Y_8"
#define PLANE_CENTER_Z_8 "PLANE_CENTER_Z_8"

#define PLANE_CENTER_X_9 "PLANE_CENTER_X_9"
#define PLANE_CENTER_Y_9 "PLANE_CENTER_Y_9"
#define PLANE_CENTER_Z_9 "PLANE_CENTER_Z_9"

#define PLANE_CENTER_X_10 "PLANE_CENTER_X_10"
#define PLANE_CENTER_Y_10 "PLANE_CENTER_Y_10"
#define PLANE_CENTER_Z_10 "PLANE_CENTER_Z_10"

#define PLANE_NORMAL_X_1 "PLANE_NORMAL_X_1"
#define PLANE_NORMAL_Y_1 "PLANE_NORMAL_Y_1"
#define PLANE_NORMAL_Z_1 "PLANE_NORMAL_Z_1"

#define PLANE_NORMAL_X_2 "PLANE_NORMAL_X_2"
#define PLANE_NORMAL_Y_2 "PLANE_NORMAL_Y_2"
#define PLANE_NORMAL_Z_2 "PLANE_NORMAL_Z_2"

#define PLANE_NORMAL_X_3 "PLANE_NORMAL_X_3"
#define PLANE_NORMAL_Y_3 "PLANE_NORMAL_Y_3"
#define PLANE_NORMAL_Z_3 "PLANE_NORMAL_Z_3"

#define PLANE_NORMAL_X_4 "PLANE_NORMAL_X_4"
#define PLANE_NORMAL_Y_4 "PLANE_NORMAL_Y_4"
#define PLANE_NORMAL_Z_4 "PLANE_NORMAL_Z_4"

#define PLANE_NORMAL_X_5 "PLANE_NORMAL_X_5"
#define PLANE_NORMAL_Y_5 "PLANE_NORMAL_Y_5"
#define PLANE_NORMAL_Z_5 "PLANE_NORMAL_Z_5"

#define PLANE_NORMAL_X_6 "PLANE_NORMAL_X_6"
#define PLANE_NORMAL_Y_6 "PLANE_NORMAL_Y_6"
#define PLANE_NORMAL_Z_6 "PLANE_NORMAL_Z_6"

#define PLANE_NORMAL_X_7 "PLANE_NORMAL_X_7"
#define PLANE_NORMAL_Y_7 "PLANE_NORMAL_Y_7"
#define PLANE_NORMAL_Z_7 "PLANE_NORMAL_Z_7"

#define PLANE_NORMAL_X_8 "PLANE_NORMAL_X_8"
#define PLANE_NORMAL_Y_8 "PLANE_NORMAL_Y_8"
#define PLANE_NORMAL_Z_8 "PLANE_NORMAL_Z_8"

#define PLANE_NORMAL_X_9 "PLANE_NORMAL_X_9"
#define PLANE_NORMAL_Y_9 "PLANE_NORMAL_Y_9"
#define PLANE_NORMAL_Z_9 "PLANE_NORMAL_Z_9"

#define PLANE_NORMAL_X_10 "PLANE_NORMAL_X_10"
#define PLANE_NORMAL_Y_10 "PLANE_NORMAL_Y_10"
#define PLANE_NORMAL_Z_10 "PLANE_NORMAL_Z_10"

#define MAX_R "MAX_R"

#define PROCESS_I "PROCESS_I"
#define PROCESS_C "PROCESS_C"
#define PROCESS_N "PROCESS_N"
#define PROCESS_TWC "PROCESS_TWC"

#define PROCESS_I1 "PROCESS_I1"
#define PROCESS_I2 "PROCESS_I2"
#define PROCESS_I3 "PROCESS_I3"
#define PROCESS_I4 "PROCESS_I4"
#define PROCESS_I5 "PROCESS_I5"
#define PROCESS_I6 "PROCESS_I6"
#define PROCESS_I7 "PROCESS_I7"
#define PROCESS_I8 "PROCESS_I8"
#define PROCESS_I9 "PROCESS_I9"
#define PROCESS_I10 "PROCESS_I10"
#define PROCESS_I11 "PROCESS_I11"
#define PROCESS_I12 "PROCESS_I12"
#define PROCESS_I13 "PROCESS_I13"
#define PROCESS_I14 "PROCESS_I14"
#define PROCESS_I15 "PROCESS_I15"
#define PROCESS_I16 "PROCESS_I16"
#define PROCESS_I17 "PROCESS_I17"
#define PROCESS_I18 "PROCESS_I18"
#define PROCESS_I19 "PROCESS_I19"
#define PROCESS_I20 "PROCESS_I20"

#define ReBuildModelName1 "ReBuildModelName1"
#define ReBuildModelName2 "ReBuildModelName2"
#define ReBuildModelName3 "ReBuildModelName3"
#define ReBuildModelName4 "ReBuildModelName4"
#define ReBuildModelName5 "ReBuildModelName5"
#define ReBuildModelName6 "ReBuildModelName6"
#define ReBuildModelName7 "ReBuildModelName7"
#define ReBuildModelName8 "ReBuildModelName8"
#define ReBuildModelName9 "ReBuildModelName9"
#define ReBuildModelName10 "ReBuildModelName10"

#define LM_HEIGHT "LM_HEIGHT"
#define JITOU_NUM "JITOU_NUM"
#define LAYER_NUM "LAYER_NUM"
#define ALTER_STL_PATH "ALTER_STL_PATH"



#define JT1_LTable "JT1_LTable"
#define JT1_CTable "JT1_CTable"
#define JT1_RTable "JT1_RTable"

#define LM_TULEN "LM_TULEN"
#define LM_TUSIDE "LM_TUSIDE"
#define JM_LEN "JM_LEN"
#define JM_WIDE "JM_WIDE"
#define JM_AOSIDE "JM_AOSIDE"



#define LM_UPLEN "LM_UPLEN"
#define LM_DOWNLEN "LM_DOWNLEN"
#define LM_R "LM_R"
#define LM_HOLE_R "LM_HOLE_R"


#define  JT1_FILES_PATH "JT1_FILES_PATH"
#define  JT2_FILES_PATH "JT2_FILES_PATH"
#define  SAFE_DIS "SAFE_DIS"



#define PRODUCTION_VALUES_FILES_PATH "PRODUCTION_VALUES_FILES_PATH"
#define ANIMATION_FILES_PATH "ANIMATION_FILES_PATH"
#define TWLC_FILES_PATH "TWLC_FILES_PATH"
#define LANGUAGE_FILE_PATH "LANGUAGE_FILE_PATH"
#define JT1_FILES_PATH "JT1_FILES_PATH"
#define JT2_FILES_PATH "JT2_FILES_PATH"

//加工扩展值文件名
#define PROCESS_EXTENTION_FILE_NAME       "PROCESS_EXTENTION_FILE_NAME" 
//PT文件名 
#define PROCESS_PT_FILE_NAME       "PROCESS_PT_FILE_NAME"

#define SEG_TYPE         "SEG_TYPE"


#endif 