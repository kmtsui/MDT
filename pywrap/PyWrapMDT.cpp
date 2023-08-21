#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "HitTube.h"
#include "HitTubeCollection.h"
#include "Configuration.h"
#include "TriggerInfo.h"
#include "MDTManager.h"

namespace py = pybind11;

PYBIND11_MODULE(MDT, m){
py::class_<HitTube>(m, "HitTube")
	.def(py::init<int>())
	.def("GetTubeID", &HitTube::GetTubeID)
	.def("GetNDigiHits", &HitTube::GetNDigiHits)
	.def("GetTimeDigi", &HitTube::GetTimeDigi)
	.def("GetChargeDigi", &HitTube::GetChargeDigi);


py::class_<HitTubeCollection>(m, "HitTubeCollection")
	.def(py::init<>())
	.def("AddTrueHit", &HitTubeCollection::AddTrueHit)
	.def("AddDigiHit", &HitTubeCollection::AddDigiHit)
	.def("GetNumOfHitTubes", &HitTubeCollection::GetNumOfHitTubes)
	.def("GetTotalNumOfTrueHits", &HitTubeCollection::GetTotalNumOfTrueHits)
	.def("GetTotalNumOfDigiHits", &HitTubeCollection::GetTotalNumOfDigiHits)
	.def("GetHitTubeIDList", &HitTubeCollection::GetHitTubeIDList)
	.def("Begin", &HitTubeCollection::Begin)
	.def("IsEnd", &HitTubeCollection::IsEnd)
	.def("Next", &HitTubeCollection::Next)
	.def("GetHitTube", &HitTubeCollection::GetHitTube, py::return_value_policy::reference);

py::class_<Configuration, std::unique_ptr<Configuration, py::nodelete>>(m, "Configuration")
	.def(py::init(&Configuration::GetInstance), py::return_value_policy::reference)
	.def("ReadParameter", &Configuration::ReadParameter)
	.def("PrintParameters", &Configuration::PrintParameters);

py::class_<TriggerInfo>(m, "TriggerInfo")
	.def(py::init<>())
	.def("GetNumOfTrigger", &TriggerInfo::GetNumOfTrigger)
	.def("GetTriggerTime", &TriggerInfo::GetTriggerTime)
	.def("GetLowEdge", &TriggerInfo::GetLowEdge)
	.def("GetUpEdge", &TriggerInfo::GetUpEdge)
	.def("GetType", &TriggerInfo::GetType);

py::class_<MDTManager>(m, "MDTManager")
	.def(py::init<int>())
	.def("DoAddDark", &MDTManager::DoAddDark)
	.def("DoDigitize", &MDTManager::DoDigitize)
	.def("DoTrigger", &MDTManager::DoTrigger)
	.def("DoInitialize", &MDTManager::DoInitialize)
	.def("GetTriggerInfo", &MDTManager::GetTriggerInfo, py::return_value_policy::reference)
	.def("GetHitTubeCollection", &MDTManager::GetHitTubeCollection, py::return_value_policy::reference);
}
