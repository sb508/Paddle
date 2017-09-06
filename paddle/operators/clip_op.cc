/* Copyright (c) 2016 PaddlePaddle Authors. All Rights Reserve.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include "paddle/operators/clip_op.h"

namespace paddle {
namespace operators {

using framework::Tensor;

class ClipOp : public framework::OperatorWithKernel {
 public:
  using framework::OperatorWithKernel::OperatorWithKernel;

 protected:
  void InferShape(const framework::InferShapeContext &ctx) const override {
    auto x_dims = ctx.Input<Tensor>("X")->dims();
    auto max = GetAttr<float>("max");
    auto min = GetAttr<float>("min");
    PADDLE_ENFORCE_LT(min, max, "max should be greater than min.");
    ctx.Output<Tensor>("Out")->Resize(x_dims);
  }
};

class ClipOpMaker : public framework::OpProtoAndCheckerMaker {
 public:
  ClipOpMaker(framework::OpProto *proto, framework::OpAttrChecker *op_checker)
      : OpProtoAndCheckerMaker(proto, op_checker) {
    AddInput("X", "The input of clip op");
    AddOutput("Out", "The output of clip op");
    AddComment(R"DOC(
Clip Operator.
)DOC");
    AddAttr<float>("min", "min value to be clipped.");
    AddAttr<float>("max", "max value to be clipped.");
  }
};

class ClipOpGrad : public framework::OperatorWithKernel {
 public:
  using framework::OperatorWithKernel::OperatorWithKernel;

 protected:
  void InferShape(const framework::InferShapeContext &ctx) const override {
    PADDLE_ENFORCE_NOT_NULL(ctx.InputVar("X"), "Input(X) should not be null");
    PADDLE_ENFORCE_NOT_NULL(ctx.InputVar(framework::GradVarName("Out")),
                            "Input(Out@GRAD) should not be null");
    auto x_dims = ctx.Input<Tensor>("X")->dims();
    auto *x_grad = ctx.Output<Tensor>(framework::GradVarName("X"));

    x_grad->Resize(x_dims);
  }
};

}  // namespace operators
}  // namespace paddle

namespace ops = paddle::operators;
REGISTER_OP(clip, ops::ClipOp, ops::ClipOpMaker, clip_grad, ops::ClipOpGrad);
REGISTER_OP_CPU_KERNEL(clip,
                       ops::ClipKernel<paddle::platform::CPUPlace, float>);
REGISTER_OP_CPU_KERNEL(clip_grad, ops::ClipGradKernel<float>);
