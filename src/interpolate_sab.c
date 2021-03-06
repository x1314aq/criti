//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"


void
interpolate_sab(nuclide_t *nuc,
                nuclide_t *sab_nuc,
                nuc_xs_t *cur_nuc_xs,
                double erg)
{
    int min, max;
    int NE_el, NE_inel;
    int sab_n_el, sab_n_inel;
    double SIG_sab_el, SIG_sab_inel, sab_k_el, sab_k_inel;

    /////////////// calculate the s(a,b) inelastic scattering cross section ///////////////
    NE_inel = (int) (sab_nuc->XSS[Get_loc_of_sab_inel_erg(sab_nuc)]);
    min = Get_loc_of_sab_inel_erg(sab_nuc) + 1;
    max = Get_loc_of_sab_inel_erg(sab_nuc) + NE_inel;
    sab_n_inel = get_intplt_pos_fr(sab_nuc->XSS, erg, min, max, &sab_k_inel);

    SIG_sab_inel = sab_nuc->XSS[NE_inel + sab_n_inel] +
        sab_k_inel * (sab_nuc->XSS[NE_inel + sab_n_inel + 1] - sab_nuc->XSS[NE_inel + sab_n_inel]);


    /////////////// calculate the s(a,b) elastic scattering cross section ///////////////
    if(Get_loc_of_sab_el_erg(sab_nuc) == 0)
        SIG_sab_el = 0;
    else {
        NE_el = (int) (sab_nuc->XSS[Get_loc_of_sab_el_erg(sab_nuc)]);
        min = Get_loc_of_sab_el_erg(sab_nuc) + 1;

        if(Get_sab_el_mode(sab_nuc) == 4 && erg <= sab_nuc->XSS[min])
            SIG_sab_el = 0;
        else {
            max = Get_loc_of_sab_el_erg(sab_nuc) + NE_el;
            sab_n_el = get_intplt_pos_fr(sab_nuc->XSS, erg, min, max, &sab_k_el);
            if(Get_sab_el_mode(sab_nuc) == 4)
                SIG_sab_el = sab_nuc->XSS[NE_el + sab_n_el] / erg;
            else {
                SIG_sab_el = sab_nuc->XSS[NE_el + sab_n_el] +
                    sab_k_el * (sab_nuc->XSS[NE_el + sab_n_el + 1] - sab_nuc->XSS[NE_el + sab_n_el]);
            }
        }
    }

    //if(Nuclides[nuclide_sab]->ProbTableFlag==1){printf("TreatURR conflict with sab \n");}
    int nn = cur_nuc_xs->inter_pos;
    double kk = cur_nuc_xs->inter_frac;
    int NE = Get_erg_grid_num(nuc);
    cur_nuc_xs->abs = nuc->XSS[nn + 2 * NE] + kk * (nuc->XSS[nn + 2 * NE + 1] - nuc->XSS[nn + 2 * NE]);
    cur_nuc_xs->tot = cur_nuc_xs->abs + SIG_sab_el + SIG_sab_inel;
    cur_nuc_xs->el = SIG_sab_el;
    cur_nuc_xs->inel = SIG_sab_inel;
    cur_nuc_xs->fis = 0;
}