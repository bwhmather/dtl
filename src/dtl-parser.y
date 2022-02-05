%skeleton "lalr1.cc"
%require "3.8.1"
%header

%define api.token.raw

%token String

%%
expression : String

%%
