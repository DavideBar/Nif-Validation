/************************************************************
   Funcoes para validar numeros de bilhete de identidade,
   contribuinte, identificacao bancaria seguranca social,
   cartao de credito e isbn.
   Segundo contribuicao de _kk_, B.Baixo, Jorge Buescu,
   Michael Gilleland (Merriam Park Software), Filipe Polido,
   Jeremy Bradbury e Hugo Pires (DRI/DRO, IIESS).
   Estas rotinas sao do dominio publico (sem copyright).

   versao 0.17, 2009/Nov.10
*************************************************************/

/************************************************************
   Changes:
     Nome das funções locais passou a não ser iniciado por
       underscore ('_')
     O primeiro dígito de um NIF pode ser 8 (João Correia)
     Correccao de bug na funcao controlCreditCard()
     controlCreditCard() e __sumLists() simplificadas
     Adicionada função para somar produto de membros de duas
       lista, __sumLists(), e outras beneficiações
     Adicionada funcao para validar numero de segurança social:
       controlNISS()
     Adicionada funcao para validar IBAN (apenas Portugal),
       segundo sugestao e contribuicao de Paula Vaz
     Alterada a funcao controlNIB() conforme codigo em
       http://download-uk.oracle.com/appsnet/115finpor.pdf
       contribuicao de Pedro Graca segundo sugestao de
       Francisco Pereira
     Adicionada funcao para validar ISBN: controlISBN()
     Adicionada funcao __toInt() para converter string
       array de inteiros
     Funcoes verificam validade de cada digito. Controlada a
       validade do primeiro digito do NIF, contribuicao de
       Nuno Anes
     Corrigida a funcao controlCreditCard(char*) segundo
       sugestao de Pedro Graca.
     Simplificada a funcao controlCreditCard(char*)
     Adicionada funcao para verificar validade de numero de
       cartao de credito: controlCreditCard(char*)
     Funcao int controlNBI(char*, char)
       corrigida por Pedro Graca e Antonio Manuel Dias
     Funcao int valN(char*) corrigida por Pedro Graca.
*************************************************************/


#include <stdio.h>
#include <string.h>
#include <ctype.h>


#define LEN_NBI   8  /* tam num bilhete identidade */
#define LEN_NIF   9  /* tam num contribuinte */
#define LEN_NISS  11 /* tam num seguranca social */
#define LEN_NIB   21 /* tam NIB */
#define LEN_ISBN  10 /* tam ISBN */

#define MINLEN_CC 7  /* tam minimo num cartao de credito */
#define MAXLEN_CC 19 /* tam maximo num cartao de credito */


/*
  Converte string passada para array de inteiros,
  eliminando todos os caracteres invalidos.
  Recebe string com numero a converter e apontador para array
  de inteiros, juntamente com o seu tamanho maximo, onde sera
  guardado o resultado. Ultimo parametro indica se 'X' e 'x'
  devem ser convertidos para '10' ou nao.
*/
int toInt(char *numstr, int *res,
             int reslen, int acceptX)
{
  int size = 0;

  /* converter todos os digitos */
  for(; *numstr; numstr++) {
    if(isdigit(*numstr)) {
      if(size == reslen)
        return -1;
      *(res++) = *numstr - '0';
      size++;
    }
  }

  /* converter digito de controlo no ISBN */
  if(acceptX) {
    numstr--;
    if(*numstr == 'X' || *numstr == 'x') {
      if(size == reslen)
        return -1;
      *res = 10;
      size++;
    }
  }

  return size;
}


/*
  Algoritmo para verificar validade de NBI e NIF.
  Recebe string com numero a validar.
*/
int valN(char *strnum)
{
  int pos;
  int val = 0;
  int num[LEN_NIF];

  /* converter strnum (string) para array de inteiros */
  if(toInt(strnum, num, LEN_NIF, 0) != LEN_NIF)
    return 0;

  /* computar soma de controlo */
  for(pos = 0; pos < LEN_NIF-1; pos++)
    val += num[pos] * (9 - pos);

  val = val % 11 ? (11 - val % 11) % 10 : 0;

  /* verificar soma de controlo */
  return val == num[pos];
}


/*
  Devolve soma dos produtos, membro a membro, das listas.
  Recebe duas listas de inteiros e tamanho das listas.
*/
int sumLists(int *a, int *b, int size)
{
  int val = 0;
  while(size--)
    val += *a++ * *b++;

  return val;
}


/*
  Verifica validade do numero do bilhete de identidade.
  Recebe string com numero BI e caracter com digito de controlo.
*/
int controlNBI(char *nbi, char control)
{
  char num[10];

  /* verificar tamanho de NBI */
  if(strlen(nbi) != LEN_NBI && strlen(nbi) != (LEN_NBI - 1))
    return 0;

  /* compor string com NBI e digito de controlo
     adicionar zero a esquerda se nbi for de comprimento 7 */
  sprintf(num, "%8s%c", nbi, control);
  if(*num == ' ')
    *num = '0';

  /* verificar validade do NBI */
  return valN(num);
}


/*
  Verifica validade de numero de contribuite.
  Recebe string com NIF.
*/
int controlNIF(char *nif)
{
  /* verificar validade do caracter inicial do NIF */
  switch(*nif) {
    case '1': case '2': case '5':
    case '6': case '8': case '9':
      /* verificar validade do NIF */
      return valN(nif);
  }

  return 0;
}


/*
  Verifica validade de numero de identificacao da seguranca
  social.  Recebe string com NISS.
*/
int controlNISS(char *strniss)
{
  int niss[LEN_NISS];
  int table[] = { 29, 23, 19, 17, 13, 11, 7, 5, 3, 2 };

  /* verificar validade do digito inicial */
  if (*strniss != '1' && *strniss != '2')
      return 0;

  /* converter para lista de inteiros */
  if(toInt(strniss, niss, LEN_NISS, 0) != LEN_NISS)
    return 0;

  /* ultimo digito e o valor de verificacao */
  return niss[LEN_NISS-1] ==
         9 - sumLists(niss, table, LEN_NISS-1) % 10;
}


/*
  Verifica validade de numero de identificacao bancaria.
  Recebe string com NIB.
*/
int controlNIB(char *strnib)
{
  int nib[LEN_NIB];
  int table[] = { 73, 17, 89, 38, 62, 45, 53, 15, 50,
                  5, 49, 34, 81, 76, 27, 90, 9, 30, 3 };

  /* converter para lista de inteiros */
  if(toInt(strnib, nib, LEN_NIB, 0) != LEN_NIB)
    return 0;

  /* ultimos dois digitos sao o valor de verificacao */
  return nib[LEN_NIB-2] * 10 + nib[LEN_NIB-1] ==
         98 - sumLists(nib, table, LEN_NIB-2) % 97;
}


/*
  Verifica validade de numero de identificacao bancaria
  internacional (apenas Portugal).
  Recebe string com IBAN.
*/
int controlIBAN(char *iban)
{
  char PT[] = "PT50\0";
  char *tmp = PT;

  /* verificar codigo IBAN para Portugal */
  while(*tmp)
    if(*tmp++ != *iban++)
      return -1;               /* -1: codigo nao suportado */

  return controlNIB(iban); 
}


/*
  Verifica a validade de numero de cartao de credito.
  Recebe string com numero do cartao de credito.
 */
int controlCreditCard(char *strncc)
{
  int sum = 0, alt = 0;
  int i, ncc[MAXLEN_CC];

  /* converter numero para lista de inteiros */
  i = toInt(strncc, ncc, MAXLEN_CC, 0);

  /* verificar tamanho do numero */
  if(MINLEN_CC > i || i > MAXLEN_CC)
    return 0;

  /* computar soma de controlo */
  for (; i--; alt = !alt) {
    if(alt) {
      ncc[i] *= 2;
      if(ncc[i] > 9)
        ncc[i] -= 9;
    }
    sum += ncc[i];
  }

  /* verificar soma de controlo */
  return !(sum % 10);
}


/*
  Verifica a validade de ISBN.
*/
int controlISBN(char *strisbn)
{
  int i;
  int sum = 0;
  int isbn[LEN_ISBN];

  /* converter para lista de inteiros */
  if(toInt(strisbn, isbn, LEN_ISBN, 1) != LEN_ISBN)
    return 0;

  /* computar soma de controlo */
  for(i = 0; i < LEN_ISBN-1; i++)
    sum += ((i+1) * isbn[i]);

  /* verificar soma de controlo */
  return sum % 11 == isbn[LEN_ISBN-1];
}



/************************************************************
** Rotina de teste
************************************************************/

int main()
{
  printf("\nTestar BI 10039784-0: %d\n",
         controlNBI("10039784", '0'));

  printf("Testar BI 6617084-2: %d\n",
         controlNBI("6617084", '2'));

  printf("Testar contribuinte 204716624: %d\n",
         controlNIF("204716624"));

  printf("Testar seguranca social 11234567892: %d\n",
         controlNISS("11234567892"));

  printf("Testar NIB 0018.0000.40359330001.87: %d\n",
         controlNIB("0018.0000.40359330001.87"));

  printf("Testar IBAN PT50.0018.0000.40359330001.87: %d\n",
         controlIBAN("PT50.0018.0000.40359330001.87"));

  printf("Testar cartao de credito 1234 5678 9999 9993: %d\n",
         controlCreditCard("1234 5678 9999 9993"));
  printf("Testar ISBN 972-662-792-3: %d\n",
         controlISBN("972-662-792-3"));

  printf("Testar ISBN 0-471-54891-X: %d\n\n",
         controlISBN("0-471-54891-X"));

  return 0;
}

