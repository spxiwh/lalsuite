/*________________________________________________________________________
 *
 * File Name: MakeTemplateBank.c
 *
 * Author: Hanna C. R.
 *
 * Revision: $Id$
 *
 *________________________________________________________________________
 */
                                                                                                                                                
/* ------------------------------- AUTO-DOC --------------------------- */
/* -------------------------------------------------------------------- */
                                                                                                                                                
                                                                                                                                                
/*<lalVerbatim file="MakeTemplateBankCV">
  Author: Hanna, C. R.
  $Id$
  </lalVerbatim>*/
                                                                                                                                                
/*SUBSECTION - MODULE - "MakeTemplateBank.c" --------------- <lalLaTeX>
  \subsection{Module \texttt{MakeTemplateBank.c}}
  \label{ss:MakeTemplateBank}
  </lalLaTeX> */
                                                                                                                                                
  /* SUBSUBSECTION - PROTOTYPES - "LALMakeTemplateBank()" ----- <lalLaTeX>
     \subsubsection{Prototypes}
     \input{LALMakeTemplateBankCP}
     \idx{MakeTemplateBank()}
     </lalLaTeX>
     END SUBSUBSECTION - PROTOTYPES "LALMakeTemplateBank()" ----------- */
                                                                                                                                                
                                                                                                                                                
  /* SUBSUBSECTION - DESCRIPTION ------------------------------ <lalLaTeX>
     \subsubsection{Description}
     </lalLaTeX>
     END SUBSUBSECTION - DESCRIPTION ---------------------------------- */
                                                                                                                                                
                                                                                                                                                
  /* SUBSUBSECTION - NOTES ------------------------------------ <lalLaTeX>
     \subsubsection{Notes}
     \begin{itemize}
     \item No notes yet.
     \end{itemize}
     </lalLaTeX>
     END SUBSUBSECTION - NOTES ---------------------------------------- */
                                                                                                                                                
/*END - SUBSECTION - MODULE - MakeTemplateBank.c" ------------------ */
                                                                                                                                                
/*<lalLaTeX>
\vfill{\footnotesize\input{MakeTemplateBankCV}}
</lalLaTeX>*/
                                                                                                                                                
                                                                                                                                                
/* ------------------------ END AUTO DOC ------------------------------ */
/* -------------------------------------------------------------------- */
                                                                                                                                                




#include<lal/LALStdlib.h>
#include<lal/LALStatusMacros.h>
#include<lal/InspiralBankGeneration.h>
#include<lal/LIGOMetadataTables.h>

NRCSID(MAKETEMPLATEBANKC, "$Id$");
   
void
/* <lalVerbatim file="LALMakeTemplateBankCP"> */
LALMakeTemplateBank(
     LALStatus *status,
     TemplateBankType *type,
     MakeTemplateBankInput *input,
     MetadataTable *table)
/* </lalVerbatim> */
{
  INITSTATUS(status, "LALMakeTemplateBank", MAKETEMPLATEBANKC);
  ATTATCHSTATUSPTR(status); 
  if(type == NULL){
    ABORT(status, TEMPLATEBANKGENERATIONH_ENULL, TEMPLATEBANKGENERATIONH_MSGENULL);
    }
  if(table == NULL){
    ABORT(status, TEMPLATEBANKGENERATIONH_ENULL, TEMPLATEBANKGENERATIONH_MSGENULL);
    }
  if (input == NULL){
    ABORT(status, TEMPLATEBANKGENERATIONH_ENULL, TEMPLATEBANKGENERATIONH_MSGENULL);
    }

/* look at inspiral Searches */
  if ((*type >= 100) && (*type < 200)){
    printf("\nInside if type statement in MakeTemplateBank\n");  
    TRY(LALInspiralBankGeneration(status->statusPtr, 
                              type, 
                              input->InspiralInput, 
                              table->snglInspiralTable), status); 
    printf("Just called LALInspiralBankGeneration\n");
    }



  /* 
  if ((*type >= 200) && (*type < 300)){
    LALPulsarBankGeneration(status->statusPointer, type,
                            input.PulsarInput, 
                            table.pulsarTable); 
  }

  
  if ((*type >= 300) && (*type < 400)){
    LALBurstBankGeneration(status->statusPointer, type,
                           input.BurstInput, 
                           table.snglBurstTable);
  }*/
  
  DETATCHSTATUSPTR(status);
  RETURN(status);

} /* LALMakeTemplateBank() */




 
