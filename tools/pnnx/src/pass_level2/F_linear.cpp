// Copyright 2021 Tencent
// SPDX-License-Identifier: BSD-3-Clause

#include "pass_level2.h"

namespace pnnx {

class F_linear : public GraphRewriterPass
{
public:
    const char* match_pattern_graph() const
    {
        return R"PNNXIR(7767517
5 4
pnnx.Input              input_0     0 1 input
pnnx.Input              input_1     0 1 weight
pnnx.Input              input_2     0 1 bias
aten::linear            op_0        3 1 input weight bias out
pnnx.Output             output      1 0 out
)PNNXIR";
    }

    const char* type_str() const
    {
        return "F.linear";
    }
};

REGISTER_GLOBAL_PNNX_GRAPH_REWRITER_PASS(F_linear, 111)

class F_linear_1 : public GraphRewriterPass
{
public:
    const char* match_pattern_graph() const
    {
        return R"PNNXIR(7767517
8 7
pnnx.Input              input_0     0 1 input
pnnx.Input              input_1     0 1 weight
pnnx.Input              input_2     0 1 bias
aten::t                 op_0        1 1 weight 9
aten::matmul            op_1        2 1 input 9 a
prim::Constant          op_2        0 1 19 value=1
aten::add               op_3        3 1 a bias 19 out
pnnx.Output             output      1 0 out
)PNNXIR";
    }

    const char* type_str() const
    {
        return "F.linear";
    }
};

REGISTER_GLOBAL_PNNX_GRAPH_REWRITER_PASS(F_linear_1, 110)

class F_linear_2 : public GraphRewriterPass
{
public:
    const char* match_pattern_graph() const
    {
        return R"PNNXIR(7767517
5 4
pnnx.Input              input_0     0 1 input
pnnx.Input              input_1     0 1 weight
aten::t                 op_0        1 1 weight 9
aten::matmul            op_1        2 1 input 9 out
pnnx.Output             output      1 0 out
)PNNXIR";
    }

    const char* type_str() const
    {
        return "F.linear";
    }

    void write(Operator* op, const std::map<std::string, Parameter>& /*captured_params*/) const
    {
        op->params["bias"] = Parameter();
    }
};

REGISTER_GLOBAL_PNNX_GRAPH_REWRITER_PASS(F_linear_2, 111)

class F_linear_3 : public GraphRewriterPass
{
public:
    const char* match_pattern_graph() const
    {
        return R"PNNXIR(7767517
8 7
pnnx.Input              input_0     0 1 input
pnnx.Input              input_1     0 1 weight
pnnx.Input              input_2     0 1 bias
aten::t                 op_0        1 1 weight 14
prim::Constant          op_1        0 1 15 value=1
prim::Constant          op_2        0 1 30 value=1
aten::addmm             op_3        5 1 bias input 14 15 30 out
pnnx.Output             output      1 0 out
)PNNXIR";
    }

    const char* type_str() const
    {
        return "F.linear";
    }
};

REGISTER_GLOBAL_PNNX_GRAPH_REWRITER_PASS(F_linear_3, 111)

class F_linear_onnx : public GraphRewriterPass
{
public:
    const char* match_pattern_graph() const
    {
        return R"PNNXIR(7767517
5 4
pnnx.Input              input_0     0 1 input
pnnx.Input              input_1     0 1 weight
pnnx.Input              input_2     0 1 bias
Gemm                    gemm        3 1 input weight bias out %*=%*
pnnx.Output             output      1 0 out
)PNNXIR";
    }

    const char* type_str() const
    {
        return "F.linear";
    }

    bool match(const std::map<std::string, const Operator*>& matched_operators, const std::map<std::string, Parameter>& captured_params, const std::map<std::string, Attribute>& /*captured_attrs*/) const
    {
        if (captured_params.find("gemm.alpha") != captured_params.end())
        {
            if (captured_params.at("gemm.alpha").type != 3 || captured_params.at("gemm.alpha").f != 1.f)
                return false;
        }

        if (captured_params.find("gemm.beta") != captured_params.end())
        {
            if (captured_params.at("gemm.beta").type != 3 || captured_params.at("gemm.beta").f != 1.f)
                return false;
        }

        if (captured_params.find("gemm.transA") != captured_params.end())
        {
            if (captured_params.at("gemm.transA").type != 2 || captured_params.at("gemm.transA").i != 0)
                return false;
        }

        if (captured_params.find("gemm.transB") == captured_params.end())
            return false;

        if (captured_params.at("gemm.transB").type != 2 || captured_params.at("gemm.transB").i != 1)
            return false;

        return true;
    }

    void write(Operator* op, const std::map<std::string, Parameter>& /*captured_params*/) const
    {
    }
};

REGISTER_GLOBAL_PNNX_GRAPH_REWRITER_PASS(F_linear_onnx, 112)

class F_linear_onnx_1 : public GraphRewriterPass
{
public:
    const char* match_pattern_graph() const
    {
        return R"PNNXIR(7767517
5 4
pnnx.Input              input_0     0 1 input
pnnx.Input              input_1     0 1 bias
pnnx.Attribute          weight      0 1 weight @data=(%in_features,%out_features)f32
Gemm                    gemm        3 1 input weight bias out %*=%*
pnnx.Output             output      1 0 out
)PNNXIR";
    }

    const char* replace_pattern_graph() const
    {
        return R"PNNXIR(7767517
5 4
pnnx.Input              input_0     0 1 input
pnnx.Input              input_1     0 1 bias
pnnx.Attribute          weight      0 1 weight
F.linear                linear      3 1 input weight bias out $weight=weight
pnnx.Output             output      1 0 out
)PNNXIR";
    }

    bool match(const std::map<std::string, const Operator*>& matched_operators, const std::map<std::string, Parameter>& captured_params, const std::map<std::string, Attribute>& /*captured_attrs*/) const
    {
        if (captured_params.find("gemm.alpha") != captured_params.end())
        {
            if (captured_params.at("gemm.alpha").type != 3 || captured_params.at("gemm.alpha").f != 1.f)
                return false;
        }

        if (captured_params.find("gemm.beta") != captured_params.end())
        {
            if (captured_params.at("gemm.beta").type != 3 || captured_params.at("gemm.beta").f != 1.f)
                return false;
        }

        if (captured_params.find("gemm.transA") != captured_params.end())
        {
            if (captured_params.at("gemm.transA").type != 2 || captured_params.at("gemm.transA").i != 0)
                return false;
        }

        if (captured_params.find("gemm.transB") != captured_params.end())
        {
            if (captured_params.at("gemm.transB").type != 2 || captured_params.at("gemm.transB").i != 0)
                return false;
        }

        return true;
    }

    void write(const std::map<std::string, Operator*>& ops, const std::map<std::string, Parameter>& captured_params, const std::map<std::string, Attribute>& captured_attrs) const
    {
        const int in_features = captured_params.at("in_features").i;
        const int out_features = captured_params.at("out_features").i;

        auto weight = captured_attrs.at("weight.data").get_float32_data();
        std::vector<float> transposed_weight(in_features * out_features);
        for (int i = 0; i < out_features; i++)
        {
            float* wptr = (float*)transposed_weight.data() + in_features * i;
            for (int j = 0; j < in_features; j++)
            {
                wptr[j] = ((const float*)weight.data())[out_features * j + i];
            }
        }

        Operator* op_weight = ops.at("weight");
        op_weight->attrs["data"] = Attribute({out_features, in_features}, transposed_weight);
    }
};

REGISTER_GLOBAL_PNNX_GRAPH_REWRITER_PASS(F_linear_onnx_1, 111)

class F_linear_onnx_2 : public F_linear_onnx_1
{
public:
    const char* match_pattern_graph() const
    {
        return R"PNNXIR(7767517
4 3
pnnx.Input              input_0     0 1 input
pnnx.Attribute          weight      0 1 weight @data=(%in_features,%out_features)f32
torch.matmul            matmul      2 1 input weight out
pnnx.Output             output      1 0 out
)PNNXIR";
    }

    const char* replace_pattern_graph() const
    {
        return R"PNNXIR(7767517
4 3
pnnx.Input              input_0     0 1 input
pnnx.Attribute          weight      0 1 weight
F.linear                linear      2 1 input weight out bias=None $weight=weight
pnnx.Output             output      1 0 out
)PNNXIR";
    }
};

REGISTER_GLOBAL_PNNX_GRAPH_REWRITER_PASS(F_linear_onnx_2, 111)

class F_linear_onnx_3 : public F_linear_onnx_1
{
public:
    const char* match_pattern_graph() const
    {
        return R"PNNXIR(7767517
6 5
pnnx.Input              input_0     0 1 input
pnnx.Attribute          weight      0 1 weight @data=(%in_features,%out_features)f32
pnnx.Attribute          bias        0 1 bias @data=(%out_features)f32
torch.matmul            matmul      2 1 input weight mm
aten::add               add         2 1 mm bias out
pnnx.Output             output      1 0 out
)PNNXIR";
    }

    const char* replace_pattern_graph() const
    {
        return R"PNNXIR(7767517
5 4
pnnx.Input              input_0     0 1 input
pnnx.Attribute          weight      0 1 weight
pnnx.Attribute          bias        0 1 bias
F.linear                linear      3 1 input weight bias out $weight=weight $bias=bias
pnnx.Output             output      1 0 out
)PNNXIR";
    }

    void write(const std::map<std::string, Operator*>& ops, const std::map<std::string, Parameter>& captured_params, const std::map<std::string, Attribute>& captured_attrs) const
    {
        F_linear_onnx_1::write(ops, captured_params, captured_attrs);

        Operator* op_bias = ops.at("bias");
        op_bias->attrs["data"] = captured_attrs.at("bias.data");
    }
};

REGISTER_GLOBAL_PNNX_GRAPH_REWRITER_PASS(F_linear_onnx_3, 110)

class F_linear_onnx_4 : public F_linear_onnx_3
{
public:
    const char* match_pattern_graph() const
    {
        return R"PNNXIR(7767517
6 5
pnnx.Input              input_0     0 1 input
pnnx.Attribute          weight      0 1 weight @data=(%in_features,%out_features)f32
pnnx.Attribute          bias        0 1 bias @data=(%out_features)f32
torch.matmul            matmul      2 1 input weight mm
aten::add               add         2 1 bias mm out
pnnx.Output             output      1 0 out
)PNNXIR";
    }
};

REGISTER_GLOBAL_PNNX_GRAPH_REWRITER_PASS(F_linear_onnx_4, 110)

class F_linear_tnn : public GraphRewriterPass
{
public:
    const char* match_pattern_graph() const
    {
        return R"PNNXIR(7767517
5 4
pnnx.Input              input_0     0 1 input
pnnx.Input              input_1     0 1 weight @data=(%in_features,%out_features)f32
pnnx.Input              input_2     0 1 bias @data=(%out_features)f32
tnn.InnerProduct        op_0        3 1 input weight bias out arg0=* arg1=* arg2=0 arg3=1
pnnx.Output             output      1 0 out
)PNNXIR";
    }

    const char* type_str() const
    {
        return "F.linear";
    }
};

REGISTER_GLOBAL_PNNX_GRAPH_REWRITER_PASS(F_linear_tnn, 140)

} // namespace pnnx
