#include "prog1.h"
#include <stdio.h>

int maxargs(A_stm stm);
int maxargsexlst(A_expList expList);
int maxagrsex(A_exp exp);
int countargs(A_expList expList);

int max(int temp1, int temp2)
{
    return temp1 > temp2 ? temp1 : temp2;
}

int maxagrsex(A_exp exp)
{
    switch (exp->kind)
    {
        case A_opExp:
        {
            int temp1 = maxagrsex(exp->u.op.left);
            int temp2 = maxagrsex(exp->u.op.right);
            return max(temp1, temp2);

        }
        case A_eseqExp:
        {
            int temp3 = maxagrsex(exp->u.eseq.exp);
            int temp4 = maxargs(exp->u.eseq.stm);
            return max(temp3, temp4);

        }

        default:
            return 0;
    }
}

int maxargsexlst(A_expList expList)
{

    switch (expList->kind)
    {
        case A_pairExpList:
        {
            int temp1 = maxargsexlst(expList->u.pair.tail);
            int temp2 = maxagrsex(expList->u.pair.head);
            return max(temp1, temp2);
        }
        case A_lastExpList:
        {
            return maxagrsex(expList->u.last);
        }

        default:
            return 0;
    }
}



int maxargs(A_stm stm)
{
    //TODO: put your code here.
    switch (stm->kind)
    {
        case A_compoundStm:
        {
            int temp1 = maxargs(stm->u.compound.stm1);
            int temp2 = maxargs(stm->u.compound.stm2);
            return max(temp1, temp2);
        }
        case A_assignStm:
        {
            A_exp tempExp = stm->u.assign.exp;
            return maxagrsex(tempExp);

        }
        case A_printStm:
            return max(maxargsexlst(stm->u.print.exps), countargs(stm->u.print.exps));
        default:
            return 0;
    }
}

int countargs(A_expList expList)
{
    if (expList->kind == A_pairExpList)
    {
        return 1+countargs(expList->u.pair.tail);
    }
    else
    {
        return 1;
    }

}


typedef struct table *Table_;
struct table
{
    string id;
    int value;
    Table_ tail;
};

Table_ Table(string id, int value, struct table *tail)
{
    Table_ t = checked_malloc(sizeof(*t));
    t->id = id;
    t->value = value;
    t->tail = tail;
    return t;
}

Table_ interpStm(A_stm s, Table_ t);
Table_ update(Table_ t, string id, int value);
int lookup(Table_ t, string key);
typedef struct intAndTable *IntAndTable_;
struct intAndTable
{
    int i;
    Table_ t;
};
IntAndTable_ interpExp(A_exp e, Table_ t);
IntAndTable_ IntAndTable(int value, Table_ t)
{
    IntAndTable_ iat = checked_malloc((sizeof(*iat)));
    iat->i = value;
    iat->t = t;
    return iat;
}
IntAndTable_ interpAndPrintExp(A_exp exp, Table_ t)
{
    IntAndTable_ iat = interpExp(exp, t);
    printf("%d ", iat->i);
    return iat;
}

Table_ interpStm(A_stm stm, Table_ t)
{
    Table_ t1;
    IntAndTable_ iat;
    switch (stm->kind)
    {
        case A_compoundStm:
        {
            t1 = interpStm(stm->u.compound.stm1, t);
            return interpStm(stm->u.compound.stm2, t1);
        }
        case A_assignStm:
        {
            IntAndTable_ iat = interpExp(stm->u.assign.exp, t);
            return update(iat->t, stm->u.assign.id, iat->i);
        }
        case A_printStm:
        {
            if (stm->u.print.exps->kind == A_lastExpList)
            {
                iat = interpAndPrintExp(stm->u.print.exps->u.last, t);
            }
            else if (stm->u.print.exps->kind == A_pairExpList)
            {
                A_expList list = stm->u.print.exps;
                while (list->kind == A_pairExpList)
                {
                    iat = interpAndPrintExp(list->u.pair.head, t);
                    t = iat->t;
                    list = list->u.pair.tail;
                }
                iat = interpAndPrintExp(list->u.last, t);
                
            }
            printf("\n");
            return iat->t;

        }
        default:
        {
            printf("Statement type error\n");
            exit(1);
        }
    }
}

IntAndTable_ interpExp(A_exp exp, Table_ t)
{
    int value1;
    IntAndTable_ iat1, iat2;
    Table_ t1;
    switch (exp->kind)
    {
        case A_idExp:
        {
            value1 = lookup(t,exp->u.id);
            return IntAndTable(value1, t);
        }
        case A_numExp:
        {
            return IntAndTable(exp->u.num, t);
        }
        case A_opExp:
        {
            iat1 = interpExp(exp->u.op.left, t);
            iat2 = interpExp(exp->u.op.right,t);
            switch (exp->u.op.oper)
            {
                case A_plus:
                {
                    value1 = iat1->i + iat2->i;
                    break;
                }
                case A_minus:
                {
                    value1 = iat1->i - iat2->i;
                    break;
                }
                case A_times:

                {
                    value1 = iat1->i * iat2->i;
                    break;
                }
                case A_div:
                {
                    value1 = iat1->i / iat2->i;
                    break;
                }
                default:
                {
                    printf("Invalid operator!\n");
                    exit(1);
                }
            }
            return IntAndTable(value1, iat2->t);
        }
        case A_eseqExp:
        {
            t1 = interpStm(exp->u.eseq.stm, t);
            return interpExp(exp->u.eseq.exp, t1);
        }
        default:
        {
            printf("Invalid operation\n");
            exit(1);
        }
    }
}

void interp(A_stm stm)
{
    Table_ t=checked_malloc(sizeof(*t));
    t->id="0";
    t->tail=NULL;
    t->value=0;
    interpStm(stm,t);
    //TODO: put your code here.
}

int lookup(Table_ t, string key)
{
    Table_ tmp = t;
    while (key != tmp->id && tmp != NULL)
    {
        tmp = tmp->tail;
    }
    if (tmp == NULL)
    {
        return 0;
    }
    return tmp->value;
}

Table_ update(Table_ t, string id, int value)
{
    return Table(id, value, t);
}


