//
// Created by vital on 02.05.2022.
//

#pragma once

#include "../keynodes/InferenceKeynodes.hpp"

namespace inference
{


class FormulaClassifier
{
public:
FormulaClassifier(ScMemoryContext * ms_context);
static int NONE;
static int ATOM;
static int NEGATION;
static int CONJUNCTION;
static int DISJUNCTION;
static int IMPLICATION_EDGE;
static int IMPLICATION_TUPLE;
static int EQUIVALENCE_EDGE;
static int EQUIVALENCE_TUPLE;

int typeOfFormula(ScAddr formula);
bool isFormulaWithConst(ScAddr formula);
bool isFormulaToGenerate(ScAddr formula);

private:
ScMemoryContext * ms_context;

};

}

