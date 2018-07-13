//===- Conv.h --------------------------------------------------===//
//
//                             The ONNC Project
//
// See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ONNC_IR_COMPUTE_OPERATOR_CONV_H
#define ONNC_IR_COMPUTE_OPERATOR_CONV_H
#include <onnc/IR/ComputeOperator.h>
#include <onnc/IR/ComputeVisitor.h>
#include <onnc/IR/Compute/Attributes.h>
#include <onnc/Support/IOStream.h>

namespace onnc {

class Conv : public ComputeOperator
{
public:
  enum IOConst {
    kX = 0,
    kW = 1,
    kB = 2,
    kY = 0
  };

public:
  Conv();

  Conv(const StringAttr& pAutoPad,
       const IntsAttr& pDilations,
       const IntAttr& pGroup,
       const IntsAttr& pKernelShape,
       const IntsAttr& pPads,
       const IntsAttr& pStrides);

  ~Conv() { }

  const StringAttr& getAutoPad() const { return m_AutoPad; }

  const IntsAttr& getDilations() const { return m_Dilations; }

  const IntAttr& getGroup() const { return m_Group; }

  const IntsAttr& getKernelShape() const { return m_KernelShape; }

  const IntsAttr& getPads() const { return m_Pads; }

  const IntsAttr& getStrides() const { return m_Strides; }

  void setAutoPad(const StringAttr& pAutoPad)  { m_AutoPad = pAutoPad; }

  void setDilations(const IntsAttr& pDilations)  { m_Dilations = pDilations; }

  void setGroup(const IntAttr& pGroup)  { m_Group = pGroup; }

  void setKernelShape(const IntsAttr& pKernelShape)  { m_KernelShape = pKernelShape; }

  void setPads(const IntsAttr& pPads)  { m_Pads = pPads; }

  void setStrides(const IntsAttr& pStrides)  { m_Strides = pStrides; }

  Tensor* getInput(unsigned int pIdx) override { return static_cast<Tensor*>(m_Inputs[pIdx]); }

  const Tensor* getInput(unsigned int pIdx) const override { return static_cast<Tensor*>(m_Inputs[pIdx]); }

  Tensor* getOutput(unsigned int pIdx) override { return static_cast<Tensor*>(m_Outputs[pIdx]); }

  const Tensor* getOutput(unsigned int pIdx) const override { return static_cast<Tensor*>(m_Outputs[pIdx]); }

  Tensor* getX() { return getInput(kX); }

  Tensor* getW() { return getInput(kW); }

  Tensor* getB() { return getInput(kB); }

  Tensor* getY() { return getOutput(kY); }

  void setX(Tensor& pTensor) { m_Inputs[kX] = &pTensor; }

  void setW(Tensor& pTensor) { m_Inputs[kW] = &pTensor; }

  void setB(Tensor& pTensor) { m_Inputs[kB] = &pTensor; }

  void setY(Tensor& pTensor) { m_Outputs[kY] = &pTensor; }

  void print(std::ostream& pOS) const override;

  void accept(ComputeVisitor& pVisitor) override { pVisitor.visit(*this); }

private:
  StringAttr m_AutoPad;
  IntsAttr m_Dilations;
  IntAttr m_Group;
  IntsAttr m_KernelShape;
  IntsAttr m_Pads;
  IntsAttr m_Strides;
};

} // namespace of onnc

#endif
