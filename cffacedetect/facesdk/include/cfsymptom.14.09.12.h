#ifndef _CF_SYMPTOM_H_
#define _CF_SYMPTOM_H_
#include "cftypes.h"
#include "cf_error.h"
#include "cfimage.h"
#include "time.h"

/*! \enum CF_SYMPTOM_BLOB_DETECTOR 
	\brief тип детектора объектов 

	префикс "sbd" означает Symptom-Blob-Detector
*/
enum CF_SYMPTOM_BLOB_DETECTOR {
	sbdAbsDiff,					/** использует разницу между двумя последовательными изображениями*/
	sbdAbsDiffBackground		/** использует разницу между текущим изображением и заранее сохраненным изображением фона*/
};

/*!\brief параметры настройки модуля SYMPTOM 
   
   Эта структура описывает параметры модуля SYMPTOM\n
   доступные для просмотра и редактирования. \n
   Структура может быть измена в результате \n
   выполнения НИР СИМПОТОМ. Для гарантии \n
   ее совместимости всегда устанавливайте значение \n
   параметра size в sizeof(CF_SYMPTOM_PARAMS)\n
   \n\n
   Каждый модуль SYMPTOM ассоцирован с источником \n
   видеоданных (видеокамерой), который характеризуется \n
   размерами выходного изображения и виртуальной 
   областью контроля. 
*/
struct CF_SYMPTOM_PARAMS
{
	/*!\brief  идентификатор структуры 
	    
		size используется для идентификации этой структуры \n
		во время выполнения. 
		Всегда должен быть  = sizeof(CF_SYMPTOM_PARAMS)
	*/
	int size; 
	/*!\brief ширина изображения

		ширина изображения, которое генерирует видеокамера\n
		ассоцированная с экземпляром модуля. \n
		По умолчанию ширина изображения = 320 пикселей
	*/
	int source_width; 
	/*!\brief высота изображения 

		высота изображения, которое генерирует видеокамера\n
		ассоцированная с экземпляром модуля. \n
		По умолчанию высота изображения = 240 пикселей
	*/
	int source_height;
	
	/*!\brief левая граница

		Левая вертикальная граница виртуальной области контроля (ROI)
	*/
	int roi_left;
	/*!\brief верхняя граница

		Верхняя горизонтальная граница виртуальной области контроля (ROI)
	*/
	int roi_top;
	/*!\brief Ширина зоны контроля. 

		Ширина виртуальной области контроля (ROI). \n
		Правая вертикальная граница виртуальной области контроля 
		определяется по формуле: right = roi_left + roi_width;
	*/
	int roi_width;
	/*!\brief Высота зоны контроля. 
		
		Высота виртуальной зоны контроля (ROI).\n
		Нижняя горизонтальная граница виртуальной области контроля\n
		определяется по формуле: bottom = roi_top + roi_height;*/
	int roi_height;
	/*!\brief тип детектора объектов. 
		
		В настоящий момент реализованы два типа детектора объектов, 
		определенных в перечислителе CF_SYMPTOM_BLOB_DETECTOR. Причем 
		первый из них направлен на установление факта движения в 
		виртуальной области контроля. И не может быть использован для 
		детектирования объектов и их сопровождения. В данной версии 
		модуля он не используется. 
		*/
	CF_SYMPTOM_BLOB_DETECTOR detector;
	
	/*!\brief  минимальная площадь объекта. 

	Минимальная площадь объекта, который может быть найден с использованием
	модуля SYMPTOM. Измеряется в % от общей площади ИЗОБРАЖЕНИЯ ( не 
	виртуальной области контроля). По умолчанию принимает значение 0.07%
	*/
	float min_blob_square;
	/*!\brief  максимальная площадь объекта. 

	Минимальная площадь объекта, который может быть найден с использованием
	модуля SYMPTOM. Измеряется в % от общей площади ИЗОБРАЖЕНИЯ ( не 
	виртуальной области контроля). По умолчанию принимает значение 3.9%
	*/
	float max_blob_square;
	/*!\brief минимальное соотношение для сторон объекта 
	
	Минимальное соотношение сторон для объекта. Для вычисления соотношения\n
	сторон объекта используется формула ширина|высота. По умолчанию, 
	установлено значение 0.2*/
	float min_blob_aspect_ratio;
	/*!\brief максимальное соотношение сторон для объекта 
	
	Максимальное соотношение сторон для объекта. Для вычисления соотношения\n
	сторон объекта используется формула ширина|высота. По умолчанию, 
	установлено значение 1*/
	float max_blob_aspect_ratio;

	/*!\brief  blob's age before alarm
	
	This field indicates how many seconds the object must be present in 
	the surveillance area (ROI) before symptom generated the event.
	*/
	float min_blob_age;
	/*!\brief  blob's immoveable time before alarm
	
	This field indicates the number of seconds the object must be 
	stationary in the observation area before symptom generated the event*/
	float min_blob_age_immov;
};
/*! \enum CF_SYMPTOM_BLOB_TYPE 
	\brief Symptom BLOB object params prefix "sb" means "Symptom Blob"
	 
	 This enum describes possible types of blobs, which could be \n
	 dected by the system.
*/
enum CF_SYMPTOM_BLOB_TYPE {
	/** the blob has "unknown" type*/
	sbUnknown =-1,  
	/** the blob looks like human*/	
	sbHuman,        
	/** the blob looks lile a bag*/
	sbBag			
};

/*!\brief Description of Blob 
	
	Detailed description of the object found search system. 
	Symptom describes the objects using the following 
	parameters: id, size, location, age and type.
*/
struct CF_SYMPTOM_BLOB
{
	/*!\brief size - this stucture identifier. 
	  should be always = sizeof(CF_SYMPTOM_BLOB)
	*/
	int size;
	/*!\brief object's ID. is used for tracking
	*/
	int id;			
	/*!\brief left corner of bounding rect
	*/
	int left;		
	/*!\brief top corner of bounding rect
	*/
	int top;		
	/*!\brief width of bounding rect
	*/
	int width;		
	/*!\brief height of bounding rect
	*/
	int height;		
	/*!\brief x-coord of mass center
	*/
	int mass_x;		
	/*!\brief y-coord of mass center
	*/
	int mass_y;		
	/*!\brief  time, in sec of presenting object
	*/
	float age;      
	/*!\brief time, in seconds when object is not moving*/
	float ageos;    
	/*!\brief type of object*/
	CF_SYMPTOM_BLOB_TYPE blob_type; 
};

/*! \brief signal handler

	For signal (event) processing user must implement the function of this type.\n
	To install custom handlers should be used CFSetXXXXXSignal function.
*/
typedef  void (__stdcall *CF_SYMPTOM_SIGNAL)(CF_SYMPTOM_BLOB* blob);

/*! \brief system handle
	
	Handle of video surveillance system "Symptom"
*/
typedef CF_HANDLE CF_SYMPTOM;


extern "C"
{

/*!\fn  CF_ERROR __EXPORT_TYPE CFCreateSymptom(CF_SYMPTOM* hSymptom)
   \brief create symptom handle 
	
	Creates a system of intelligent video surveillance descriptor "symptom".
	\param hSymptom pointer to symptom handle

	Return values: CF_SUCCESS if the handle was successfully created.
	CFF_INVALID_HANDLE  if hSymptom == NULL
	CFCR_NOT_ENOUGH_MEMORY  in case of lack or damage to the memory.
	
	Created handle must be freed using the function CFDestroySymptom
*/
CF_ERROR __EXPORT_TYPE CFCreateSymptom(CF_SYMPTOM* hSymptom);

/*!\fn  CF_ERROR __EXPORT_TYPE CFDestroySymptom (CF_SYMPTOM* hSymptom)
   \brief free and destroy CF_SYMPTOM handle
	
	Release handle is created. 
	\param hSymptom - pointer to symptom handle

	Return values: CF_SUCCESS if the handle was successfully released.
	CFF_INVALID_HANDLE  if hSymptom == NULL or invalid
*/
CF_ERROR __EXPORT_TYPE CFDestroySymptom(CF_SYMPTOM* hSymptom);
/*!\fn  CF_ERROR __EXPORT_TYPE CFBeginSurv(CF_SYMPTOM hSymptom, CF_IMAGE hImage);
   \brief begin surveillance

	Starts the video. Sets the system clock. Sets the incoming video signal.
	
	\param hSymptom - symptom handle
	\param hImage - image handle

	Return values: CF_SUCCESS video surveillance was started
	CFF_INVALID_HANDLE  if hSymptom or hImage	 == NULL or invalid
*/
CF_ERROR __EXPORT_TYPE CFBeginSurv(CF_SYMPTOM hSymptom, CF_IMAGE hImage);
/*!\fn  CFAddSurvImage(CF_SYMPTOM hSymptom, CF_IMAGE hImage)
   \brief process image for surveillance
	
	Performs tracking of objects found. Finding new objects in the image 
	hImage. Analyzes the state of video scenes and generates 
	all the signals surveillance.
	
	\param hSymptom - symptom handle
	\param hImage - image handle

	Return values: CF_SUCCESS video surveillance was processed 
	CFF_INVALID_HANDLE  if hSymptom or hImage	 == NULL or invalid
*/
CF_ERROR __EXPORT_TYPE CFAddSurvImage(CF_SYMPTOM hSymptom, CF_IMAGE hImage);
/*!\fn  CFEndSurv(CF_SYMPTOM hSymptom, CF_IMAGE hImage)
   \brief finish video surveillance
	
	performs a stop surveillance. 
	Resets the clock and releases the found objects.
	all the signals surveillance.
	
	\param hSymptom - symptom handle
	\param hImage - image handle

	Return values: CF_SUCCESS image was processed 
	CFF_INVALID_HANDLE  if hSymptom or hImage	 == NULL or invalid
*/
CF_ERROR __EXPORT_TYPE CFEndSurv(CF_SYMPTOM hSymptom, CF_IMAGE hImage); 

/*!\fn  CF_ERROR __EXPORT_TYPE CFGetNumBlobs(CF_SYMPTOM hSymptom, int* pNumBlobs)
   \brief returns the number of objects found
	
	number of found objects can be> = 0
	
	\param hSymptom - symptom handle
	\param pNumBlobs - a pointer to an integer that stores the number of found objects.

	Return values: CF_SUCCESS if all ok
	CFF_INVALID_HANDLE  if hSymptom or pNumBlobs	 == NULL or invalid
	CFF_INVALID_PARAM   if pNumBlobs == NULL
*/
CF_ERROR __EXPORT_TYPE CFGetNumBlobs(CF_SYMPTOM hSymptom, int* pNumBlobs);
/*!\fn  CF_ERROR __EXPORT_TYPE CFGetBlob(CF_SYMPTOM hSymptom, int index, CF_SYMPTOM_BLOB* pBlob)
   \brief returns  the object found on the index
	
	returns CF_SYMPTOM_BLOB - full description of blob
	
	\param hSymptom - symptom handle
	\param index  - index of desired blob
	\param pBlob  - pointer to CF_SYMPTOM_BLOB, that stores the blob. 

	note: pBlob->size must be sizeof (CF_SYMPTOM_BLOB)

	Return values: CF_SUCCESS all ok
	CFF_INVALID_HANDLE  if hSymptom  == NULL or invalid
	CFF_INVALID_PARAM   if pBlob == NULL or pBlob->size != sizeof(CF_SYMPTOM_BLOB)
	or index < 0 or index > number of blobs 
*/
CF_ERROR __EXPORT_TYPE CFGetBlob(CF_SYMPTOM hSymptom, int index, CF_SYMPTOM_BLOB* pBlob);
/*!\fn  CF_ERROR __EXPORT_TYPE CFGetSurvParams(CF_SYMPTOM hSymptom, CF_SYMPTOM_PARAMS* pParams)
   \brief returns the current settings of the system
	
	returns CF_SYMPTOM_PARAMS - full description of current settings of the system
	
	\param hSymptom - symptom handle
	\param pParams - pointer to CF_SYMPTOM_PARAMS, that stores the settings. 

	note: pParams->size must be sizeof (CF_SYMPTOM_PARAMS)

	Return values: CF_SUCCESS all ok\n
	CFF_INVALID_HANDLE  if hSymptom  == NULL or invalid \n
	CFF_INVALID_PARAM   if pParams == NULL or pParams->size != sizeof(CF_SYMPTOM_PARAMS)
*/
CF_ERROR __EXPORT_TYPE CFGetSurvParams(CF_SYMPTOM hSymptom, CF_SYMPTOM_PARAMS* pParams);
/*!\fn  CF_ERROR __EXPORT_TYPE CFSetSurvParams(CF_SYMPTOM hSymptom, CF_SYMPTOM_PARAMS* pParams)
   \brief set the current settings to the system
	
	sets CF_SYMPTOM_PARAMS - full description of current settings of the system to the hSymptom
	
	\param hSymptom - symptom handle
	\param pParams - pointer to CF_SYMPTOM_PARAMS, that stores the settings. 

	note: pParams->size must be sizeof (CF_SYMPTOM_PARAMS)
	
	Return values: CF_SUCCESS all ok \n
	CFF_INVALID_HANDLE  if hSymptom  == NULL or invalid \n
	CFF_INVALID_PARAM   if pParams == NULL or pParams->size != sizeof(CF_SYMPTOM_PARAMS) \n
	or settings can not be accepted because of their inconsistency.
*/
CF_ERROR __EXPORT_TYPE CFSetSurvParams(CF_SYMPTOM hSymptom, CF_SYMPTOM_PARAMS* pParams);
/*!\fn  CF_ERROR __EXPORT_TYPE CFLoadSurvParams(const char* lpFileName, CF_SYMPTOM_PARAMS* pParams)
   \brief loads settings from xml file
	
	sets CF_SYMPTOM_PARAMS - full description of current settings of the system from xml file
	
	\param lpFileName - full name of the xml file
	\param pParams - pointer to CF_SYMPTOM_PARAMS, that stores the settings. 

	note: pParams->size must be sizeof (CF_SYMPTOM_PARAMS)
	
	Return values: CF_SUCCESS all ok \n
	CFF_INVALID_PARAM   if pParams == NULL or pParams->size != sizeof(CF_SYMPTOM_PARAMS) \n
	lpFileName == NULL.\n
	CFF_CANNOT_LOAD_PARAMS if file is invald or corrupt or has inappropriate format.
*/
CF_ERROR __EXPORT_TYPE CFLoadSurvParams(const char* lpFileName, CF_SYMPTOM_PARAMS* pParams);
/*!\fn  CF_ERROR __EXPORT_TYPE CFSaveSurvParams(const char* lpFileName, CF_SYMPTOM_PARAMS* pParams)
   \brief saves settings to xml file
	
	save CF_SYMPTOM_PARAMS - full description of current settings of the system to the xml file
	
	\param lpFileName - full name of the xml file
	\param pParams - pointer to CF_SYMPTOM_PARAMS, that stores the settings. 

	note: pParams->size must be sizeof (CF_SYMPTOM_PARAMS)
	
	Return values: CF_SUCCESS all ok \n
	CFF_INVALID_PARAM   if pParams == NULL or pParams->size != sizeof(CF_SYMPTOM_PARAMS) \n
	lpFileName == NULL.\n
	CFF_CANNOT_LOAD_PARAMS if file is invald or corrupt or has inappropriate format.
	CFF_CANNOT_SAVE_PARAMS if file name is corrupt or some other error in saving process
*/
CF_ERROR __EXPORT_TYPE CFSaveSurvParams(const char* lpFileName, CF_SYMPTOM_PARAMS* pParams);
/*!\fn  CF_ERROR __EXPORT_TYPE  CFSetNewBlobSignal(CF_SYMPTOM hSymptom, CF_SYMPTOM_SIGNAL SignalFunc)
   \brief sets event callback for NewBlob signal
	
	set user callback function  CF_SYMPTOM_SIGNAL \n
	for NewBlob signal
	
	\param hSymptom - symptom handle
	\param SignalFunc - user callback function for process NewBlob signal. 


	Return values: CF_SUCCESS all ok \n
	CFF_INVALID_HANDLE  if hSymptom  == NULL or invalid \n
*/
CF_ERROR __EXPORT_TYPE  CFSetNewBlobSignal(CF_SYMPTOM hSymptom, CF_SYMPTOM_SIGNAL SignalFunc); 
/*!\fn  CF_ERROR __EXPORT_TYPE  CFSetLostBlobSignal(CF_SYMPTOM hSymptom, CF_SYMPTOM_SIGNAL SignalFunc)
   \brief sets event callback for LostBlob signal
	
	set user callback function  CF_SYMPTOM_SIGNAL \n
	for LostBlob signal
	
	\param hSymptom - symptom handle
	\param SignalFunc - user callback function for process LostBlob signal. 


	Return values: CF_SUCCESS all ok \n
	CFF_INVALID_HANDLE  if hSymptom  == NULL or invalid \n
*/
CF_ERROR __EXPORT_TYPE  CFSetLostBlobSignal(CF_SYMPTOM hSymptom, CF_SYMPTOM_SIGNAL SignalFunc);
/*!\fn  CF_ERROR __EXPORT_TYPE  CFSetBlobPosChagedSignal(CF_SYMPTOM hSymptom, CF_SYMPTOM_SIGNAL SignalFunc)
   \brief sets event callback for BlobPosChaged signal
	
	set user callback function  CF_SYMPTOM_SIGNAL \n
	for BlobPosChaged signal
	
	\param hSymptom - symptom handle
	\param SignalFunc - user callback function for process BlobPosChaged signal. 


	Return values: CF_SUCCESS all ok \n
	CFF_INVALID_HANDLE  if hSymptom  == NULL or invalid \n
*/
CF_ERROR __EXPORT_TYPE  CFSetBlobPosChagedSignal(CF_SYMPTOM hSymptom, CF_SYMPTOM_SIGNAL SignalFunc);
/*!\fn  CF_ERROR __EXPORT_TYPE  CFSetBlobImmoveableSignal(CF_SYMPTOM hSymptom, CF_SYMPTOM_SIGNAL SignalFunc)
   \brief sets event callback for BlobImmoveable signal
	
	set user callback function  CF_SYMPTOM_SIGNAL \n
	for BlobImmoveable signal
	
	\param hSymptom - symptom handle
	\param SignalFunc - user callback function for process BlobImmoveable signal. 


	Return values: CF_SUCCESS all ok \n
	CFF_INVALID_HANDLE  if hSymptom  == NULL or invalid \n
*/
CF_ERROR __EXPORT_TYPE  CFSetBlobImmoveableSignal(CF_SYMPTOM hSymptom, CF_SYMPTOM_SIGNAL SignalFunc);
/*!\fn  CF_ERROR __EXPORT_TYPE  CFSetBlobPresentAlarmSignal(CF_SYMPTOM hSymptom, CF_SYMPTOM_SIGNAL SignalFunc)
   \brief sets event callback for BlobPresentAlarm signal
	
	set user callback function  CF_SYMPTOM_SIGNAL \n
	for BlobPresentAlarm signal
	
	\param hSymptom - symptom handle
	\param SignalFunc - user callback function for process BlobPresentAlarm signal. 


	Return values: CF_SUCCESS all ok \n
	CFF_INVALID_HANDLE  if hSymptom  == NULL or invalid \n
*/
CF_ERROR __EXPORT_TYPE  CFSetBlobPresentAlarmSignal(CF_SYMPTOM hSymptom, CF_SYMPTOM_SIGNAL SignalFunc);
/*!\fn  CF_ERROR __EXPORT_TYPE  CFSetBlobImmoveabeAlarmSignal(CF_SYMPTOM hSymptom, CF_SYMPTOM_SIGNAL SignalFunc)
   \brief sets event callback for BlobImmoveabeAlarm signal
	
	set user callback function  CF_SYMPTOM_SIGNAL \n
	for BlobImmoveabeAlarm signal
	
	\param hSymptom - symptom handle
	\param SignalFunc - user callback function for process BlobImmoveabeAlarm signal. 


	Return values: CF_SUCCESS all ok \n
	CFF_INVALID_HANDLE  if hSymptom  == NULL or invalid \n
*/
CF_ERROR __EXPORT_TYPE  CFSetBlobImmoveabeAlarmSignal(CF_SYMPTOM hSymptom, CF_SYMPTOM_SIGNAL SignalFunc);
} 
#endif //_CF_SYMPTOM_H_