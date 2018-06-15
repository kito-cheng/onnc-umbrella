//===- BM188xBackend.h ----------------------------------------------------===//
//
//                             The ONNC Project
//
// See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef BM188X_BACKEND_H
#define BM188X_BACKEND_H
#include "TGBackend.h"
#include <memory>
#include <onnc/Target/TG/BM188x/common_calibration2.pb.h>
#include <onnx/common/ir.h>
#include <string>

namespace onnc {

class TGCodeEmitter;
class TargetTransformInfo;

class BM1880Backend : public TGBackend
{
public:
  BM1880Backend(const TargetOptions &pOptions);
  ~BM1880Backend() override = default;
  bool isNativeTensorType(::onnx::TensorProto_DataType pType) override;
  std::string getBackendName() override { return "BM1880Backend"; };
  std::string getCtableName() override { return "bm1880_ctable"; }
  void setCtableProto(const std::string &pTextString) override;
  const tg::bm1880::LayerCalibrationParameter &
  getCtableLayerParam(std::string &pName);
  const TargetTransformInfo *getTTI() const override { return m_pTTI; }

private:
  tg::bm1880::NetCalibrationParameter m_NetCtableParam;
  TargetTransformInfo *m_pTTI; // NOLINT
};

} // namespace onnc

#endif // BM188X_BACKEND_H