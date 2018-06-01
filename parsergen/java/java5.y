%{

  /* This part goes to JavaParser.h */

#pragma once

#include <LRparser.h>
#include "Java5Symbol.h"
#include "Java5Lex.h"

class Java5Parser : public LRparser {
public:
  static const ParserTables *Java5Tables;
private:
  int reduceAction(unsigned int prod);

  void userStackInit() {                              // Called before the first parsecycle
  }

  void userStackShiftSymbol(unsigned int symbol) {    // Called when LRparser shift in inputtoken
  }

  void userStackPopSymbols(unsigned int count)   {    // Pop count symbols from userstack
  }

  void userStackShiftLeftSide()                  {    // Push($$) to userstack. called at the end of each reduction
  }

  void defaultReduce(unsigned int prod)          {    // $$ = $1
  }

  int *getStackTop(int fromtop) {
    return NULL;
  }
  String m_fileName;
public:
  Java5Parser(const TCHAR *fileName, Scanner *lex=NULL, unsigned int stacksize = 1000) : LRparser(*Java5Tables,lex,stacksize) {
    m_fileName = fileName;
  }
  ~Java5Parser() {
  }
};

%}

%term   CLASS INTERFACE ENUM PACKAGE IMPORT
%term   TYPEBOOLEAN TYPEBYTE TYPECHAR TYPESHORT TYPEINT TYPELONG TYPEFLOAT TYPEDOUBLE TYPEVOID
%term   FOR WHILE DO IF ELSE SWITCH CASE DEFAULT BREAK CONTINUE RETURN THROW TRYTOKEN CATCH FINALLY ASSERT
%term   PUBLIC PRIVATE PROTECTED ABSTRACT FINAL STATIC TRANSIENT SYNCHRONIZED NATIVE VOLATILE STRICTFP
%term   THROWS EXTENDS IMPLEMENTS
%term   INTEGERLITERAL FLOATLITERAL BOOLEANLITERAL CHARACTERLITERAL STRINGLITERAL NULLLITERAL THISLITERAL SUPER INSTANCEOF NEW
%left   COMMA
%term   SEMICOLON DOT LC RC LPAR RPAR LB RB ELLIPSIS AT
%term   ASSIGN PLUSASSIGN MINUSASSIGN STARASSIGN DIVASSIGN MODASSIGN SHLASSIGN SSHRASSIGN USHRASSIGN ANDASSIGN XORASSIGN ORASSIGN
%left   QUESTION COLON
%left   COMPLEMENT NOT NEQ EQ LT LE GT GE
%left   XOR OROR ANDAND 
%left   SHL SSHR USHR
%left   AND OR
%left   PLUS MINUS STAR DIV MOD
%term   PLUSPLUS MINUSMINUS
%term   NUMBER IDENTIFIER

%{

  /* This part goes to the first part of javaaction.cpp */

#include "stdafx.h"
#include "Java5Parser.h"

#pragma warning(disable:4312)

%}

%%

Goal                            : CompilationUnit
                                ;

CompilationUnit                 : PackageDeclaration? ImportDeclaration* TypeDeclaration*
                                ;

Literal                         : INTEGERLITERAL
                                | FLOATLITERAL
                                | BOOLEANLITERAL
                                | CHARACTERLITERAL
                                | STRINGLITERAL
                                | NULLLITERAL
                                ;

Type                            : PrimitiveType
                                | ReferenceType
                                ;

PrimitiveType                   : TYPEBOOLEAN
                                | TYPEBYTE
                                | TYPECHAR
                                | TYPESHORT
                                | TYPEINT
                                | TYPELONG
                                | TYPEFLOAT
                                | TYPEDOUBLE
                                ;

/*
ReferenceTypeList               : ReferenceType
                                | ReferenceTypeList COMMA ReferenceType
                                ;
*/

ReferenceType                   : ClassOrInterfaceType
                                | PrimitiveType Dim+
                                | Name Dim+
                                | ClassOrInterfaceName TypeArguments Dim+
                                | ClassOrInterfaceName TypeArguments DOT Name Dim+
                                ;

ClassOrInterfaceTypeList        : ClassOrInterfaceType
                                | ClassOrInterfaceTypeList COMMA ClassOrInterfaceType
                                ;

ClassOrInterfaceType            : ClassOrInterfaceName TypeArguments?
                                ;

ClassOrInterfaceName            : Name
                                | ClassOrInterfaceName TypeArguments DOT Name
                                ;

/*
GenericIdentifier               : Name TypeArguments?
                                ;
*/

Name                            : Identifier
                                | Name DOT Identifier
                                ;

Identifier                      : IDENTIFIER
                                ;


PackageDeclaration              : Modifier* PACKAGE Name SEMICOLON
                                ;

ImportDeclaration               : SingleTypeImportDeclaration
                                | TypeImportOnDemandDeclaration
                                | SingleStaticImportDeclaration
                                | StaticImportOnDemandDeclaration
                                ;

SingleTypeImportDeclaration     : IMPORT Name SEMICOLON
                                ;

TypeImportOnDemandDeclaration   : IMPORT Name DOT STAR SEMICOLON 
                                ;

SingleStaticImportDeclaration   : IMPORT STATIC Name SEMICOLON
                                ;

StaticImportOnDemandDeclaration : IMPORT STATIC Name DOT STAR SEMICOLON
                                ;

TypeDeclaration                 : ClassDeclaration
                                | EnumDeclaration
                                | InterfaceDeclaration
                                | AnnotationTypeDeclaration
                                | SEMICOLON
                                ;

Modifier                        : Annotation
                                | PUBLIC 
                                | PROTECTED
                                | PRIVATE
                                | STATIC
                                | ABSTRACT
                                | FINAL
                                | NATIVE
                                | SYNCHRONIZED
                                | TRANSIENT
                                | VOLATILE
                                | STRICTFP
                                ;

ClassDeclaration                : Modifier* CLASS Identifier TypeParameters? ExtendsClass? ImplementsInterfaces? ClassBody
                                ;

ExtendsClass                    : EXTENDS ClassOrInterfaceType
                                ;

ImplementsInterfaces            : IMPLEMENTS ClassOrInterfaceTypeList
                                ;

ClassBody                       : LC ClassBodyDeclaration* RC
                                ;

ClassBodyDeclaration            : ClassMemberDeclaration
                                | StaticInitializer
                                | ConstructorDeclaration
                                | Block
                                ;

ClassMemberDeclaration          : FieldDeclaration
                                | MethodDeclaration
                                | TypeDeclaration
                                ;

StaticInitializer               : STATIC Block
                                ;

FieldDeclaration                : Modifier* Type VariableDeclarators SEMICOLON
                                ;

MethodDeclaration               : Modifier* TypeParameters? Type MethodName FormalParameters Dim* ThrowsException? MethodBody
                                | Modifier* TypeParameters? TYPEVOID MethodName FormalParameters ThrowsException? MethodBody
                                ;

ConstructorDeclaration          : Modifier* TypeParameters? MethodName FormalParameters ThrowsException? ConstructorBody
                                ;

Block                           : LC BlockStatement* RC
                                ;

MethodName						: Identifier
								;

VariableDeclarators             : VariableDeclarator
                                | VariableDeclarators COMMA VariableDeclarator
                                ;

VariableDeclarator              : VariableDeclaratorId
                                | VariableDeclaratorId ASSIGN VariableInitializer
                                ;

VariableDeclaratorId            : Identifier Dim*
                                ;

VariableInitializer             : Expression
                                | ArrayInitializer
                                ;

FormalParameters                : LPAR FormalParameterList? RPAR
                                ;

FormalParameterList             : FormalParameter
                                | FormalParameterList COMMA FormalParameter
                                ;

FormalParameter                 : Modifier* Type EllipsisOperator? VariableDeclaratorId
                                ;

EllipsisOperator                : ELLIPSIS
                                ;

ThrowsException                 : THROWS ClassOrInterfaceTypeList
                                ;

MethodBody                      : Block
                                | SEMICOLON
                                ;

ConstructorBody                 : LC ExplicitContructorInvocation? BlockStatement* RC
                                ;

ExplicitContructorInvocation    : TypeArguments? THISLITERAL Arguments SEMICOLON
                                | TypeArguments? SUPER Arguments SEMICOLON
                                | Primary DOT TypeArguments? SUPER Arguments SEMICOLON
                                ;

InterfaceDeclaration            : Modifier* INTERFACE Identifier TypeParameters? ExtendsInterfaces? InterfaceBody
                                ;

ExtendsInterfaces               : EXTENDS ClassOrInterfaceTypeList
                                ;

InterfaceBody                   : LC InterfaceMemberDeclaration* RC
                                ;

InterfaceMemberDeclaration      : ConstantDeclaration
                                | AbstractMethodDeclaration
                                | TypeDeclaration
                                ;

EnumDeclaration                 : Modifier* ENUM Identifier ImplementsInterfaces? EnumBody
                                ;

EnumBody                        : LC EnumConstantList? COMMA? EnumBodyDeclarations? RC
                                ;

EnumConstantList                : EnumConstant
                                | EnumConstantList COMMA EnumConstant
                                ;

EnumConstant                    : Annotation* Identifier Arguments? ClassBody?
                                ;

EnumBodyDeclarations            : SEMICOLON ClassBodyDeclaration*
                                ;

AnnotationTypeDeclaration       : Modifier* AT INTERFACE Identifier AnnotationTypeBody
                                ;

AnnotationTypeBody              : LC AnnotationTypeElementDeclaration* RC
                                ;

AnnotationTypeElementDeclaration: Modifier* Type Identifier LPAR RPAR DefaultValue? SEMICOLON
                                | ConstantDeclaration
                                | TypeDeclaration
                                ;

DefaultValue                    : DEFAULT ElementValue
                                ;

Arguments                       : LPAR ArgumentList? RPAR
                                ;

ArgumentList                    : Expression
                                | ArgumentList COMMA Expression
                                ;

ConstantDeclaration             : FieldDeclaration
                                ;

AbstractMethodDeclaration       : Modifier* TypeParameters? Type MethodName FormalParameters Dim* ThrowsException? SEMICOLON
                                | Modifier* TypeParameters? TYPEVOID MethodName FormalParameters ThrowsException? SEMICOLON
                                ;

ArrayInitializer                : LC VariableInitializerList? COMMA? RC
                                ;

VariableInitializerList         : VariableInitializer
                                | VariableInitializerList COMMA VariableInitializer
                                ;

BlockStatement                  : LocalVariableDeclarationStatement
                                | Statement
                                | ClassDeclaration
                                | EnumDeclaration
                                ;

LocalVariableDeclarationStatement : LocalVariableDeclaration SEMICOLON
                                ;

LocalVariableDeclaration        : Modifier* Type VariableDeclarators
                                ;

Statement                       : StatementWithoutTrailingSubstatement
                                | LabeledStatement
                                | IfThenStatement
                                | IfThenElseStatement
                                | WhileStatement
                                | ForStatement
                                | EnhancedForStatement
                                ;

StatementNoShortIf              : StatementWithoutTrailingSubstatement
                                | LabeledStatementNoShortIf
                                | IfThenElseStatementNoShortIf
                                | WhileStatementNoShortIf
                                | ForStatementNoShortIf
                                | EnhancedForStatementNoShortIf
                                ;

StatementWithoutTrailingSubstatement
                                : Block
                                | EmptyStatement
                                | ExpressionStatement
                                | SwitchStatement
                                | DoStatement
                                | BreakStatement
                                | ContinueStatement
                                | ReturnStatement
                                | SynchronizedStatement
                                | ThrowStatement
                                | TryStatement
                                | AssertStatementShort
                                | AssertStatementLong
                                ;

EmptyStatement                  : SEMICOLON
                                ;

LabeledStatement                : Identifier COLON Statement
                                ;

LabeledStatementNoShortIf       : Identifier COLON StatementNoShortIf
                                ;

ExpressionStatement             : StatementExpression SEMICOLON
                                ;

StatementExpression             : Assignment
                                | PreIncrementExpression
                                | PreDecrementExpression
                                | PostIncrementExpression
                                | PostDecrementExpression
                                | MethodInvocation
                                | NewClassExpression
                                ;

IfThenStatement                 : IF Condition Statement
                                ;

IfThenElseStatement             : IF Condition StatementNoShortIf ELSE Statement
                                ;

IfThenElseStatementNoShortIf    : IF Condition StatementNoShortIf ELSE StatementNoShortIf
                                ;

SwitchStatement                 : SWITCH Condition SwitchBlock
                                ;

SwitchBlock                     : LC SwitchBlockStatementGroup* SwitchLabel* RC
                                ;

SwitchBlockStatementGroup       : SwitchLabel+ BlockStatement+
                                ;

SwitchLabel                     : CASE ConstantExpression COLON
                                | DEFAULT COLON
                                ;

WhileStatement                  : WHILE Condition Statement
                                ;

WhileStatementNoShortIf         : WHILE Condition StatementNoShortIf
                                ;

DoStatement                     : DO Statement WHILE Condition SEMICOLON
                                ;

ForStatement                    : FOR LPAR ForInit? SEMICOLON Expression? SEMICOLON ForUpdate? RPAR Statement
                                ;

ForStatementNoShortIf           : FOR LPAR ForInit? SEMICOLON Expression? SEMICOLON ForUpdate? RPAR StatementNoShortIf
                                ;

ForInit                         : StatementExpressionList
                                | LocalVariableDeclaration
                                ;

ForUpdate                       : StatementExpressionList
                                ;

EnhancedForStatement            : FOR LPAR Modifier* Type VariableDeclaratorId COLON Expression RPAR Statement
                                ;

EnhancedForStatementNoShortIf   : FOR LPAR Modifier* Type VariableDeclaratorId COLON Expression RPAR StatementNoShortIf
                                ;

StatementExpressionList         : StatementExpression
                                | StatementExpressionList COMMA StatementExpression
                                ;

BreakStatement                  : BREAK Identifier? SEMICOLON
                                ;

ContinueStatement               : CONTINUE Identifier? SEMICOLON
                                ;

ReturnStatement                 : RETURN Expression? SEMICOLON
                                ;

ThrowStatement                  : THROW Expression SEMICOLON
                                ;

SynchronizedStatement           : SYNCHRONIZED Condition Block
                                ;

TryStatement                    : TRYTOKEN Block CatchClause+
                                | TRYTOKEN Block CatchClause* FinallyClause
                                ;

CatchClause                     : CATCH FormalParameters Block
                                ;

FinallyClause                   : FINALLY Block
                                ;

AssertStatementShort            : ASSERT Expression SEMICOLON
                                ;

AssertStatementLong             : ASSERT Expression COLON Expression SEMICOLON
                                ;

Condition                       : LPAR Expression RPAR
                                ;

Primary                         : PrimaryNoNewArray
                                | NewArrayExpression
                                ;

PrimaryNoNewArray               : Literal
                                | THISLITERAL
                                | LPAR Name RPAR
                                | LPAR ExpressionNN RPAR
                                | NewClassExpression
                                | FieldAccess
                                | MethodInvocation
                                | ArrayAccess
                                | Name DOT THISLITERAL
                                | Name DOT SUPER
                                | PrimitiveType Dim* DOT CLASS
                                | Name Dim* DOT CLASS
                                | TYPEVOID DOT CLASS
                                ;

NewClassExpression              : NEW TypeArguments? ClassOrInterfaceType Arguments ClassBody?
                                | Primary DOT NEW TypeArguments? Identifier Arguments ClassBody?
                                | Name DOT NEW TypeArguments? Identifier Arguments ClassBody?
                                ;

NewArrayExpression              : NEW PrimitiveType DimExpression+ Dim*
                                | NEW ClassOrInterfaceType DimExpression+ Dim*
                                | NEW PrimitiveType Dim+ ArrayInitializer
                                | NEW ClassOrInterfaceType Dim+ ArrayInitializer
                                ;

FieldAccess                     : Primary DOT Identifier
                                | SUPER   DOT Identifier
                                ;

MethodInvocation                : Identifier Arguments
                                | Name DOT TypeArguments? Identifier Arguments
                                | Primary DOT TypeArguments? Identifier Arguments
                                | SUPER   DOT TypeArguments? Identifier Arguments
                                ;

ArrayAccess                     : Name DimExpression
                                | PrimaryNoNewArray DimExpression
                                ;

DimExpression                   : LB Expression RB
                                ;

Dim                             : LB RB
                                ;

ConstantExpression              : Expression
                                ;

Expression                      : AssignmentExpression
                                ;

ExpressionNN                    : AssignmentExpressionNN
                                ;

AssignmentExpression            : ConditionalExpression
                                | Assignment
                                ;

AssignmentExpressionNN          : ConditionalExpressionNN
                                | Assignment
                                ;

Assignment                      : PostfixExpression AssignmentOperator AssignmentExpression
                                ;

ConditionalExpression           : ShortOrExpression
                                | ShortOrExpression QUESTION Expression COLON ConditionalExpression
                                ;

ConditionalExpressionNN         : ShortOrExpressionNN
                                | ShortOrExpressionNN QUESTION Expression COLON ConditionalExpression
                                | Name QUESTION Expression COLON ConditionalExpression
                                ;

ShortOrExpression               : ShortAndExpression
                                | ShortOrExpression OROR ShortAndExpression
                                ;

ShortOrExpressionNN             : ShortAndExpressionNN
                                | ShortOrExpressionNN OROR ShortAndExpression
                                | Name OROR ShortAndExpression
                                ;

ShortAndExpression              : OrExpression
                                | ShortAndExpression ANDAND OrExpression
                                ;

ShortAndExpressionNN            : OrExpressionNN
                                | ShortAndExpressionNN ANDAND OrExpression
                                | Name ANDAND OrExpression
                                ;

OrExpression                    : XorExpression
                                | OrExpression OR XorExpression
                                ;

OrExpressionNN                  : XorExpressionNN
                                | OrExpressionNN OR XorExpression
                                | Name OR XorExpression
                                ;

XorExpression                   : AndExpression
                                | XorExpression XOR AndExpression
                                ;

XorExpressionNN                 : AndExpressionNN
                                | XorExpressionNN XOR AndExpression
                                | Name XOR AndExpression
                                ;

AndExpression                   : EqualityExpression
                                | AndExpression AND EqualityExpression
                                ;

AndExpressionNN                 : EqualityExpressionNN
                                | AndExpressionNN AND EqualityExpression
                                | Name AND EqualityExpression
                                ;

EqualityExpression              : InstanceOfExpression
                                | EqualityExpression EQ InstanceOfExpression
                                | EqualityExpression NEQ InstanceOfExpression
                                ;

EqualityExpressionNN            : InstanceOfExpressionNN
                                | EqualityExpressionNN EQ InstanceOfExpression
                                | Name EQ InstanceOfExpression
                                | EqualityExpressionNN NEQ InstanceOfExpression
                                | Name NEQ InstanceOfExpression
                                ;

InstanceOfExpression            : RelationalExpression
                                | InstanceOfExpression INSTANCEOF ReferenceType
                                ;

InstanceOfExpressionNN          : RelationalExpressionNN
                                | InstanceOfExpressionNN INSTANCEOF ReferenceType
                                | Name INSTANCEOF ReferenceType
                                ;

RelationalExpression            : ShiftExpression
                                | RelationalExpression LT ShiftExpression
                                | RelationalExpression GT ShiftExpression
                                | RelationalExpression LE ShiftExpression
                                | RelationalExpression GE ShiftExpression
                                ;

RelationalExpressionNN          : ShiftExpressionNN
                                | RelationalExpressionNN LT ShiftExpression
                                | Name LT ShiftExpression
                                | RelationalExpressionNN GT ShiftExpression
                                | Name GT ShiftExpression
                                | RelationalExpressionNN LE ShiftExpression
                                | Name LE ShiftExpression
                                | RelationalExpressionNN GE ShiftExpression
                                | Name GE ShiftExpression
                                ;

ShiftExpression                 : AdditiveExpression
                                | ShiftExpression SHL AdditiveExpression
                                | ShiftExpression SSHR AdditiveExpression
                                | ShiftExpression USHR AdditiveExpression
                                ;

ShiftExpressionNN               : AdditiveExpressionNN
                                | ShiftExpressionNN SHL AdditiveExpression
                                | Name SHL AdditiveExpression
                                | ShiftExpressionNN SSHR AdditiveExpression
                                | Name SSHR AdditiveExpression
                                | ShiftExpressionNN USHR AdditiveExpression
                                | Name USHR AdditiveExpression
                                ;

AdditiveExpression              : MultiplicativeExpression
                                | AdditiveExpression PLUS MultiplicativeExpression
                                | AdditiveExpression MINUS MultiplicativeExpression
                                ;

AdditiveExpressionNN            : MultiplicativeExpressionNN
                                | AdditiveExpressionNN PLUS MultiplicativeExpression
                                | Name PLUS MultiplicativeExpression
                                | AdditiveExpressionNN MINUS MultiplicativeExpression
                                | Name MINUS MultiplicativeExpression
                                ;
                                
MultiplicativeExpression        : UnaryExpression
                                | MultiplicativeExpression STAR UnaryExpression
                                | MultiplicativeExpression DIV UnaryExpression
                                | MultiplicativeExpression MOD UnaryExpression
                                ;

MultiplicativeExpressionNN      : UnaryExpressionNN
                                | MultiplicativeExpressionNN STAR UnaryExpression
                                | Name STAR UnaryExpression
                                | MultiplicativeExpressionNN DIV UnaryExpression
                                | Name DIV UnaryExpression
                                | MultiplicativeExpressionNN MOD UnaryExpression
                                | Name MOD UnaryExpression
                                ;

UnaryExpression                 : PreIncrementExpression
                                | PreDecrementExpression
                                | PLUS UnaryExpression
                                | MINUS UnaryExpression
                                | UnaryExpressionNotPlusMinus
                                ;

UnaryExpressionNN               : PreIncrementExpression
                                | PreDecrementExpression
                                | PLUS UnaryExpression
                                | MINUS UnaryExpression
                                | UnaryExpressionNotPlusMinusNN
                                ;

PreIncrementExpression          : PLUSPLUS UnaryExpression
                                ;

PreDecrementExpression          : MINUSMINUS UnaryExpression
                                ;

UnaryExpressionNotPlusMinus     : PostfixExpression
                                | COMPLEMENT UnaryExpression
                                | NOT UnaryExpression
                                | CastExpression
                                ;

UnaryExpressionNotPlusMinusNN   : PostfixExpressionNN
                                | COMPLEMENT UnaryExpression
                                | NOT UnaryExpression
                                | CastExpression
                                ;

CastExpression                  : LPAR PrimitiveType RPAR UnaryExpression
                                | LPAR PrimitiveType Dim+ RPAR UnaryExpression
                                | LPAR Name TypeArguments RPAR UnaryExpressionNotPlusMinus
                                | LPAR Name TypeArguments Dim+ RPAR UnaryExpressionNotPlusMinus
                                | LPAR Name TypeArguments DOT ClassOrInterfaceType RPAR UnaryExpressionNotPlusMinus
                                | LPAR Name TypeArguments DOT ClassOrInterfaceType Dim+ RPAR UnaryExpressionNotPlusMinus
                                | LPAR Name RPAR UnaryExpressionNotPlusMinus
                                | LPAR Name Dim+ RPAR UnaryExpressionNotPlusMinus
                                ;

PostfixExpression               : Primary
                                | Name
                                | PostIncrementExpression
                                | PostDecrementExpression
                                ;

PostfixExpressionNN             : Primary
                                | PostIncrementExpression
                                | PostDecrementExpression
                                ;

PostIncrementExpression         : PostfixExpression PLUSPLUS
                                ;

PostDecrementExpression         : PostfixExpression MINUSMINUS
                                ;

AssignmentOperator              : ASSIGN
                                | STARASSIGN
                                | DIVASSIGN
                                | MODASSIGN
                                | PLUSASSIGN
                                | MINUSASSIGN
                                | SHLASSIGN
                                | SSHRASSIGN
                                | USHRASSIGN
                                | ANDASSIGN
                                | XORASSIGN
                                | ORASSIGN
                                ;

TypeArguments                   : LT TypeArgumentListGT
                                ;

TypeArgumentListGT              : TypeArgumentGT
                                | TypeArgumentList COMMA TypeArgumentGT
                                ;

TypeArgumentGT                  : ReferenceTypeGT
                                | WildcardGT
                                ;

ReferenceTypeGT                 : ReferenceType GT
                                | ClassOrInterfaceName LT TypeArgumentListSSHR
                                ;

WildcardGT                      : QUESTION GT
                                | QUESTION EXTENDS ReferenceTypeGT
                                | QUESTION SUPER ReferenceTypeGT
                                ;
                                                                
TypeArgumentListSSHR            : TypeArgumentSSHR
                                | TypeArgumentList COMMA TypeArgumentSSHR
                                ;
                                                                
TypeArgumentSSHR                : ReferenceTypeSSHR
                                | WildcardSSHR
                                ;
                                                                
ReferenceTypeSSHR               : ReferenceType SSHR
                                | ClassOrInterfaceName LT TypeArgumentListUSHR
                                ;

WildcardSSHR                    : QUESTION SSHR
                                | QUESTION EXTENDS ReferenceTypeSSHR
                                | QUESTION SUPER ReferenceTypeSSHR
                                ;
                                                                
TypeArgumentListUSHR            : TypeArgumentUSHR
                                | TypeArgumentList COMMA TypeArgumentUSHR
                                ;
                                                                
TypeArgumentUSHR                : ReferenceTypeUSHR
                                | WildcardUSHR
                                ;
                                                                
ReferenceTypeUSHR               : ReferenceType USHR
                                ;

WildcardUSHR                    : QUESTION USHR
                                | QUESTION EXTENDS ReferenceTypeUSHR
                                | QUESTION SUPER ReferenceTypeUSHR
                                ;

TypeArgumentList                : TypeArgument
                                | TypeArgumentList COMMA TypeArgument
                                ;

TypeArgument                    : ReferenceType
                                | Wildcard
                                ;

Wildcard                        : QUESTION
                                | QUESTION EXTENDS ReferenceType
                                | QUESTION SUPER ReferenceType
                                ;

TypeParameters                  : LT TypeParameterListGT
                                ;

TypeParameterListGT             : TypeParameterGT
                                | TypeParameterList COMMA TypeParameterGT
                                ;

TypeParameterGT                 : Identifier GT
                                | Identifier TypeBoundGT
                                ;

TypeBoundGT                     : EXTENDS ReferenceTypeGT
                                | EXTENDS ReferenceType AdditionalBoundSequenceGT
                                ;

AdditionalBoundSequenceGT       : AdditionalBoundGT
                                | AdditionalBoundSequenceGT AdditionalBoundGT
                                ;

AdditionalBoundGT               : AND ReferenceTypeGT
                                ;

TypeParameterList               : TypeParameter
                                | TypeParameterList COMMA TypeParameter
                                ;

TypeParameter                   : Identifier TypeBound?
                                ;

TypeBound                       : EXTENDS ReferenceType
                                | EXTENDS ReferenceType AdditionalBoundSequence
                                ;

AdditionalBoundSequence         : AdditionalBound
                                | AdditionalBoundSequence AdditionalBound
                                ;

AdditionalBound                 : AND ReferenceType
                                ;

Annotation                      : NormalAnnotation
                                | MarkerAnnotation
                                | SingleElementAnnotation
                                ;

NormalAnnotation                : AT Name LPAR ElementValuePairList? RPAR
                                ;

MarkerAnnotation                : AT Name
                                ;

SingleElementAnnotation         : AT Name LPAR ElementValue RPAR
                                ;

ElementValuePairList            : ElementValuePair
                                | ElementValuePairList COMMA ElementValuePair
                                ;

ElementValuePair                : Identifier ASSIGN ElementValue
                                ;

ElementValueList                : ElementValue
                                | ElementValueList COMMA ElementValue
                                ;

ElementValue                    : Name
                                | ConditionalExpressionNN
                                | Annotation
                                | ElementValueArrayInitializer
                                ;

ElementValueArrayInitializer    : LC ElementValueList? COMMA? RC
                                ;

%%

