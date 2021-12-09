/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <utility>
#include <vector>
#include <sc-memory/sc_template.hpp>
#include <manager/TemplateManager.hpp>
#include "searcher/TemplateSearcher.hpp"

struct LogicExpressionResult {
    bool result;
    bool hasTemplateSearchResult;
    ScTemplateSearchResultItem templateSearchResult{nullptr, nullptr};
    ScAddr templateItself;
};

class LogicExpressionNode {
public:
    virtual LogicExpressionResult check(ScTemplateParams params) const = 0;

    virtual std::string toString() const = 0;
};

class AndExpressionNode : public LogicExpressionNode {
public:
    AndExpressionNode(std::unique_ptr<LogicExpressionNode> opA,
                      std::unique_ptr<LogicExpressionNode> opB);

    LogicExpressionResult check(ScTemplateParams params) const override;

    std::string toString() const {
        return "AND(" + leftOp->toString() + ", " + rightOp->toString() + ")";
    }

private:
    std::unique_ptr<LogicExpressionNode> leftOp;
    std::unique_ptr<LogicExpressionNode> rightOp;
};

class OrExpressionNode : public LogicExpressionNode {
public:
    OrExpressionNode(std::unique_ptr<LogicExpressionNode> opA,
                     std::unique_ptr<LogicExpressionNode> opB);

    LogicExpressionResult check(ScTemplateParams params) const override;

    std::string toString() const {
        return "OR(" + leftOp->toString() + ", " + rightOp->toString() + ")";
    }

private:
    std::unique_ptr<LogicExpressionNode> leftOp;
    std::unique_ptr<LogicExpressionNode> rightOp;
};

class NotExpressionNode : public LogicExpressionNode {
public:
    explicit NotExpressionNode(std::unique_ptr<LogicExpressionNode> op);

    LogicExpressionResult check(ScTemplateParams params) const override;

    std::string toString() const {
        return "NOT( " + op->toString() + ")";
    }

private:
    std::unique_ptr<LogicExpressionNode> op;
};

class TemplateExpressionNode : public LogicExpressionNode {
public:
    TemplateExpressionNode(ScAddr templateToCheck,
                           inference::TemplateSearcher* templateSearcher,
                           ScMemoryContext* context);

    LogicExpressionResult check(ScTemplateParams params) const override;

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
                    inference::TemplateSearcher* templateSearcher,
                    inference::TemplateManager * templateManager,
                    std::vector<ScAddr> argumentList);

    void build();

    std::string toString() {
        return root->toString();
    }

    std::unique_ptr<LogicExpressionNode> build(ScAddr node);

    const std::vector<ScTemplateParams>& GetParamsSet() const {
        return paramsSet;
    }

    LogicExpressionNode& GetRoot() {
        return *root;
    }

private:
    std::vector<ScTemplateParams> paramsSet;
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