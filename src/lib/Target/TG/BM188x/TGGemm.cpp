#define DEBUG_TYPE "tg_gemm"

#include "TGGemm.h"
#include "BM188xCodeEmitter.h"
#include <bmkernel_api.h>
#include <onnc/Support/Debug.h>

namespace onnc {
namespace BM188X {

// TGGemm
// Y = alpha * A * B + beta * C
// where input tensor A has dimension (M X K) , input tensor B has dimension (K
// X N), input tensor C and output tensor Y have dimension (M X N).
TGGemm::TGGemm(const ::onnx::Node &pNode,
               const tg::bm1880::LayerCalibrationParameter &pLayerCtable)
    : ComputeOperator2(pNode, "Gemm"), m_InRowNum(0), m_InColNum(0),
      m_OutColNum(0), m_HaveBias(0), m_WeightTp(false),
      m_LayerCtable(pLayerCtable)
{

  const std::vector< ::onnx::Dimension> aDim = pNode.inputs()[0]->sizes();
  const std::vector< ::onnx::Dimension> bDim = pNode.outputs()[0]->sizes();
  m_InRowNum = aDim[0].dim;
  m_InColNum = aDim[1].dim;
  if (aDim.size() == 4) {
    m_InColNum *= aDim[2].dim * aDim[3].dim;
  }
  m_OutColNum = bDim[1].dim;
  m_HaveBias = true;

  if (pNode.hasAttribute(::onnx::Symbol("transB"))) {
    auto transB = pNode.i(::onnx::Symbol("transB"));
    DEBUG(dbgs() << "transB:" << transB << std::endl;);
    m_WeightTp = true;
  }
}

TGGemm *TGGemm::addMemOperands(MemOperand *pInput, MemOperand *pOutput,
                               MemOperand *pWeight, MemOperand *pBias)
{
  m_MemOperands.push_back(pInput);
  m_MemOperands.push_back(pWeight);
  m_MemOperands.push_back(pBias);
  m_MemOperands.push_back(pOutput);
  return this;
}

void TGGemm::print(OStream &pOS) const
{
  int rShiftWidth = m_LayerCtable.right_shift_width();
  pOS << *m_MemOperands[3] << " = Gemm <inRowNum:" << m_InRowNum
      << ", inColNum:" << m_InColNum << ", outColNum:" << m_OutColNum
      << ", m_WeightTp:" << m_WeightTp << ", lShiftWidth:0"
      << ", rShiftWidth:" << rShiftWidth << "> (" << *m_MemOperands[0] << ", "
      << *m_MemOperands[1] << ", " << *m_MemOperands[2] << ")\n";
}

void TGGemm::emit() const
{
  DEBUG(print(dbgs()));
  int rShiftWidth = m_LayerCtable.right_shift_width();
  bmnet::bmnet_fc_fixed_forward_bmkernel(
      *bm1880_kernel::getInstance().m_CTX,
      m_MemOperands[0]->m_Addr, // input_data_gaddr
      m_MemOperands[1]->m_Addr, // weight_data_gaddr
      m_MemOperands[2]->m_Addr, // bias_data_gaddr
      m_MemOperands[3]->m_Addr, // output_data_gaddr
      m_InRowNum,               // input_row_num
      m_InColNum,               // input_col_num
      m_OutColNum,              // weight_col_num
      m_HaveBias,               // have_bias
      0,                        // do_activation
      0,                        // activation_method
      GADDR_INVALID,            // activation_ga_slope
      0,                        // activation_channel_shared
      0,                        // activation_gt_scale
      0,                        // activation_gt_rshift
      0,                        // activation_le_scale
      0,                        // activation_le_rshift
      m_WeightTp,               // weight_transpose
      0,                        // left_shift_width //TODO
      rShiftWidth               // right_shift_width
  );
}
void TGGemm::toASM(tg::bm1880::Insn *pI) const
{
  // bool m_WeightTp;
  pI->set_name(getLayerName());
  pI->set_type(tg::bm1880::Insn::GEMM);
  auto *gemm = pI->mutable_gemm_param();
  assert(m_MemOperands.size() == 4);
  {
    auto *input = gemm->mutable_input();
    bm_asm::setDim(input, m_InRowNum, m_InColNum);
    bm_asm::setMem(input, m_MemOperands.at(0), tg::bm1880::Operand::Int8);
  }
  {
    auto *weight = gemm->mutable_weight();
    bm_asm::setDim(weight, m_InColNum, m_OutColNum);
    bm_asm::setMem(weight, m_MemOperands.at(1), tg::bm1880::Operand::Int8);
    weight->set_transpose(m_WeightTp);
  }
  if (m_HaveBias) {
    auto *bias = gemm->mutable_bias();
    bm_asm::setDim(bias, m_InRowNum, m_OutColNum);
    bm_asm::setMem(bias, m_MemOperands.at(2), tg::bm1880::Operand::Int16);
  }
  {
    auto *output = gemm->mutable_output();
    bm_asm::setDim(output, m_InRowNum, m_OutColNum);
    bm_asm::setMem(output, m_MemOperands.at(3), tg::bm1880::Operand::Int8);
  }
}
} // namespace BM188X
} // namespace onnc