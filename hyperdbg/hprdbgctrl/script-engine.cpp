/**
 * @file script-engine.cpp
 * @author Sina Karvandi (sina@rayanfam.com)
 * @brief Interpret script engine affairs
 * @details
 * @version 0.1
 * @date 2021-09-23
 *
 * @copyright This project is released under the GNU Public License v3.
 *
 */
#include "pch.h"

//
// Global Variables
//
extern UINT64  g_ResultOfEvaluatedExpression;
extern UINT32  g_ErrorStateOfResultOfEvaluatedExpression;
extern BOOLEAN g_IsSerialConnectedToRemoteDebuggee;

/**
 * @brief Get the value from the evaluation of single expression
 * from local debuggee and remote debuggee
 *
 * @param Expr
 * @param HasError
 * @return UINT64
 */
UINT64
ScriptEngineEvalSingleExpression(string Expr, PBOOLEAN HasError)
{
    PVOID  CodeBuffer;
    UINT64 BufferAddress;
    UINT32 BufferLength;
    UINT32 Pointer;
    UINT64 Result = NULL;

    //
    // Check if it's connected over remote debuggee (in the Debugger Mode)
    //
    if (g_IsSerialConnectedToRemoteDebuggee)
    {
        //
        // Send over serial
        //
        //
        // Prepend and append 'print(' and ')'
        //
        Expr.insert(0, "formats(");
        Expr.append(");");

        //
        // TODO: end of string must have a whitspace. fix it.
        //
        Expr.append(" ");
        // Expr = " x = 4 >> 1; ";

        //
        // Run script engine handler
        //
        CodeBuffer = ScriptEngineParseWrapper((char *)Expr.c_str());

        if (CodeBuffer == NULL)
        {
            //
            // return to show that this item contains an script
            //
            *HasError = TRUE;
            return NULL;
        }

        //
        // Print symbols (test)
        //
        // PrintSymbolBufferWrapper(CodeBuffer);

        //
        // Set the buffer and length
        //
        BufferAddress = ScriptEngineWrapperGetHead(CodeBuffer);
        BufferLength  = ScriptEngineWrapperGetSize(CodeBuffer);
        Pointer       = ScriptEngineWrapperGetPointer(CodeBuffer);

        //
        // Send it to the remote debuggee
        //
        KdSendScriptPacketToDebuggee(BufferAddress, BufferLength, Pointer, TRUE);

        //
        // Remove the buffer of script engine interpreted code
        //
        ScriptEngineWrapperRemoveSymbolBuffer(CodeBuffer);

        //
        // Check whether there was an error in evaluation or not
        //
        if (g_ErrorStateOfResultOfEvaluatedExpression == DEBUGEER_OPERATION_WAS_SUCCESSFULL)
        {
            //
            // Everything was fine, return the result of the evaluated
            // expression and null the global holders
            //
            Result                                    = g_ResultOfEvaluatedExpression;
            g_ErrorStateOfResultOfEvaluatedExpression = NULL;
            g_ResultOfEvaluatedExpression             = NULL;
            *HasError                                 = FALSE;
            return Result;
        }
        else
        {
            g_ErrorStateOfResultOfEvaluatedExpression = NULL;
            g_ResultOfEvaluatedExpression             = NULL;

            *HasError = TRUE;
            return NULL;
        }
    }
    else
    {
        //
        // It's in vmi-mode
        //
        return 0;
    }
}