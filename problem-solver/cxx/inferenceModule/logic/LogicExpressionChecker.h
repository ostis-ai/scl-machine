/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <utility>
#include <vector>
#include <sc-memory/sc_template.hpp>
#include "searcher/TemplateSearcher.hpp"

class LogicExpressionNode {
public:
    virtual std::tuple<bool, bool, ScTemplateSearchResultItem, ScAddr>
            check(ScAddr referenceTemplate, ScTemplateParams params) const = 0;

    virtual std::string toString() const = 0;
};

class AndExpressionNode : public LogicExpressionNode {
public:
    AndExpressionNode(std::unique_ptr<LogicExpressionNode> opA,
                      std::unique_ptr<LogicExpressionNode> opB) :
            leftOp(std::move(opA)),
            rightOp(std::move(opB)) {}

    std::tuple<bool, bool, ScTemplateSearchResultItem, ScAddr>
        check(ScAddr referenceTemplate, ScTemplateParams params) const override {

        bool finalResult = false;

        auto result = leftOp->check(referenceTemplate, params);
        std::tuple<bool, bool, ScTemplateSearchResultItem, ScAddr> result2
            {false, false, {nullptr, nullptr}, ScAddr()};
        if (std::get<0>(result)) {
            result2 = rightOp->check(referenceTemplate, params);

            if (std::get<0>(result2))
                finalResult = true;
        }

        bool firstResultHasReferenceTemplate = std::get<1>(result);
        bool secondResultHasReferenceTemplate = std::get<1>(result2);

        return {finalResult,

            finalResult
            ?  firstResultHasReferenceTemplate || secondResultHasReferenceTemplate
            : false,

                firstResultHasReferenceTemplate ? std::get<2>(result) :
                (secondResultHasReferenceTemplate ? std::get<2>(result2) :
                ScTemplateSearchResultItem(nullptr, nullptr)),

                std::get<3>(result)
        };
    }

    std::string toString() const {
        return "AND( " + leftOp->toString() + ", " + rightOp->toString() + ")";
    }

private:
    std::unique_ptr<LogicExpressionNode> leftOp;
    std::unique_ptr<LogicExpressionNode> rightOp;
};

class OrExpressionNode : public LogicExpressionNode {
public:
    OrExpressionNode(std::unique_ptr<LogicExpressionNode> opA,
                     std::unique_ptr<LogicExpressionNode> opB) :
            leftOp(std::move(opA)),
            rightOp(std::move(opB)) {}

    std::tuple<bool, bool, ScTemplateSearchResultItem, ScAddr>
        check(ScAddr referenceTemplate, ScTemplateParams params) const override {

        bool finalResult = true;

        auto result = leftOp->check(referenceTemplate, params);
        std::tuple<bool, bool, ScTemplateSearchResultItem, ScAddr> result2
                {false, false, {nullptr, nullptr}, ScAddr()};
        if (!std::get<0>(result)) {
            result2 = rightOp->check(referenceTemplate, params);

            if (!std::get<0>(result2))
                finalResult = false;
        }

        bool firstResultHasReferenceTemplate = std::get<1>(result);
        bool secondResultHasReferenceTemplate = std::get<1>(result2);

        return {finalResult,

                finalResult
                ?  firstResultHasReferenceTemplate || secondResultHasReferenceTemplate
                : false,

                firstResultHasReferenceTemplate ? std::get<2>(result) :
                (secondResultHasReferenceTemplate ? std::get<2>(result2) :
                 ScTemplateSearchResultItem(nullptr, nullptr)),

                std::get<3>(result)
        };
    }

    std::string toString() const {
        return "OR( " + leftOp->toString() + ", " + rightOp->toString() + ")";
    }

private:
    std::unique_ptr<LogicExpressionNode> leftOp;
    std::unique_ptr<LogicExpressionNode> rightOp;
};

class NotExpressionNode : public LogicExpressionNode {
public:
    explicit NotExpressionNode(std::unique_ptr<LogicExpressionNode> op) : op(std::move(op)) {}

    std::tuple<bool, bool, ScTemplateSearchResultItem, ScAddr>
        check(ScAddr referenceTemplate, ScTemplateParams params) const override {

        auto result = op->check(referenceTemplate, params);

        std::get<0>(result) = !std::get<0>(result);

        return result;
    }

    std::string toString() const {
        return "NOT( " + op->toString() + ")";
    }

private:
    std::unique_ptr<LogicExpressionNode> op;
};

class TemplateExpressionNode : public LogicExpressionNode {
public:
    TemplateExpressionNode(ScAddr templateToCheck,
            inference::TemplateSearcher* templateSearcher, ScMemoryContext* context)
            : templateToCheck(templateToCheck), templateSearcher(templateSearcher), context(context) {}

    std::tuple<bool, bool, ScTemplateSearchResultItem, ScAddr>
    check(ScAddr referenceTemplate, ScTemplateParams params) const override {
        vector<ScTemplateSearchResultItem> searchResult =
                templateSearcher->searchTemplate(templateToCheck, params);

        SC_LOG_INFO(context->HelperGetSystemIdtf(templateToCheck) + ": "
            + std::to_string(!searchResult.empty()))

        if (!searchResult.empty()) {
            return {true, true, searchResult[0], templateToCheck};
        } else {
            return {false, false,
                    {nullptr, nullptr}, templateToCheck};
        }
    }

    std::string toString() const {
        std::string sysIdtf = context->HelperGetSystemIdtf(templateToCheck);

        return sysIdtf.empty() ? std::to_string(templateToCheck.GetRealAddr().seg) + ":" +
            std::to_string(templateToCheck.GetRealAddr().offset) : sysIdtf;
    }

private:
    ScAddr templateToCheck;
    inference::TemplateSearcher* templateSearcher;
    ScMemoryContext* context;
};

class LogicExpression {
public:
    LogicExpression(ScAddr condition, ScMemoryContext* context,
            inference::TemplateSearcher* templateSearcher, inference::TemplateManager * templateManager,
                    vector<ScAddr> argumentList)
        : conditionRoot(condition), context(context),
        templateSearcher(templateSearcher), templateManager(templateManager),
        argumentList(std::move(argumentList)){

        atomicLogicalFormula = context->HelperResolveSystemIdtf("atomic_logical_formula");
        nrelDisjunction = context->HelperResolveSystemIdtf("nrel_disjunction");
        nrelConjunction = context->HelperResolveSystemIdtf("nrel_conjunction");
        nrelNegation = context->HelperResolveSystemIdtf("nrel_negation");
    }

    void build() {
        //collect all the templates within if condition
        //generate for them sets of ScTemplateParams
        //choose the set when IsEmpty() is false, memorize the template the set was generated for

        root = build(conditionRoot);
    }

    std::string toString() {
        return root->toString();
    }

    std::unique_ptr<LogicExpressionNode> build(ScAddr node) {
        SC_LOG_INFO(context->HelperGetSystemIdtf(node))
        ScIterator3Ptr atomicFormula = context->Iterator3(
                atomicLogicalFormula,
                ScType::EdgeAccessConstPosPerm,
                node);

        ScIterator3Ptr conjunction = context->Iterator3(
                nrelConjunction,
                ScType::EdgeAccessConstPosPerm,
                node);

        ScIterator3Ptr disjunction = context->Iterator3(
                nrelDisjunction,
                ScType::EdgeAccessConstPosPerm,
                node);

        ScIterator3Ptr negation = context->Iterator3(
                nrelNegation,
                ScType::EdgeAccessConstPosPerm,
                node);

        ScIterator3Ptr operands = context->Iterator3(
                node,
                ScType::EdgeAccessConstPosPerm,
                ScType::Unknown);

        if (atomicFormula->Next()) {
            //atomic formula, make TemplateExpressionNode
            SC_LOG_INFO("template")
            std::vector<ScTemplateParams> params
                = templateManager->createTemplateParamsList(node, argumentList);
            SC_LOG_INFO("C: " + std::to_string(params.size()));
            if (!params.empty() && paramsSet.empty()) {
                //SC_LOG_INFO("ref: " + context->HelperGetSystemIdtf(referenceTemplate));
                paramsSet = std::move(params);
            }

            return std::make_unique<TemplateExpressionNode>(node, templateSearcher, context);
        } else if (conjunction->Next()) {
            SC_LOG_INFO("cojunction")

            //and, make AndExpressionNode
            if (operands->Next()) {
                std::unique_ptr<LogicExpressionNode> opA =
                        build(operands->Get(2));

                if (operands->Next()) {
                    std::unique_ptr<LogicExpressionNode> opB =
                            build(operands->Get(2));

                    return std::make_unique<AndExpressionNode>(std::move(opA),
                            std::move(opB));
                }
            }
        } else if (disjunction->Next()) {
            //or, make OrExpressionNode
            SC_LOG_INFO("disj")

            if (operands->Next()) {
                std::unique_ptr<LogicExpressionNode> opA =
                        build(operands->Get(2));

                if (operands->Next()) {
                    std::unique_ptr<LogicExpressionNode> opB =
                            build(operands->Get(2));

                    return std::make_unique<OrExpressionNode>(std::move(opA),
                            std::move(opB));
                }
            }
        } else if (negation->Next()) {
            SC_LOG_INFO("not")

            //not, make NotExpressionNode
            if (operands->Next()) {
                std::unique_ptr<LogicExpressionNode> op =
                        build(operands->Get(2));

                return std::make_unique<NotExpressionNode>(std::move(op));
            }
        }
        SC_LOG_INFO("rerr")

        throw runtime_error("[INFERENCE_MODULE] Unrecognized element of condition tree");

        return nullptr;
    }

    const std::vector<ScTemplateParams>& GetParamsSet() const {
        return paramsSet;
    }

    ScAddr GetReferenceTemplate() const {
        return referenceTemplate;
    }

    LogicExpressionNode& GetRoot() {
        return *root;
    }

private:
    std::vector<ScTemplateParams> paramsSet;
    ScAddr referenceTemplate;
    std::unique_ptr<LogicExpressionNode> root;
    ScAddr conditionRoot;
    ScMemoryContext* context;
    inference::TemplateSearcher* templateSearcher;
    inference::TemplateManager * templateManager;
    vector<ScAddr> argumentList;

    ScAddr atomicLogicalFormula;
    ScAddr nrelDisjunction;
    ScAddr nrelConjunction;
    ScAddr nrelNegation;
};