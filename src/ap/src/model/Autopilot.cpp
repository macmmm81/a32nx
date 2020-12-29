#include "Autopilot.h"
#include "Autopilot_private.h"

const uint8_T Autopilot_IN_NO_ACTIVE_CHILD = 0U;
const uint8_T Autopilot_IN_any = 1U;
const uint8_T Autopilot_IN_left = 2U;
const uint8_T Autopilot_IN_right = 3U;
const uint8_T Autopilot_IN_NO_ACTIVE_CHILD_a = 0U;
const uint8_T Autopilot_IN_OFF = 1U;
const uint8_T Autopilot_IN_ON = 2U;
const uint8_T Autopilot_IN_COND = 1U;
const uint8_T Autopilot_IN_LOC = 1U;
const uint8_T Autopilot_IN_LOC_CPT = 2U;
const uint8_T Autopilot_IN_LOC_TRK = 3U;
const uint8_T Autopilot_IN_MANAGED = 2U;
const uint8_T Autopilot_IN_SELECTED = 3U;
const uint8_T Autopilot_IN_ALT_ACQ = 1U;
const uint8_T Autopilot_IN_ALT_HOLD = 2U;
const uint8_T Autopilot_IN_GS = 3U;
const uint8_T Autopilot_IN_SPD_MACH = 4U;
const uint8_T Autopilot_IN_VS = 5U;
const ap_output Autopilot_rtZap_output = {
  {
    0.0,
    0.0
  },

  {
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0
  },

  {
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0
  },

  {
    0.0,

    {
      0.0,
      0.0,
      0.0
    },

    {
      0.0,
      0.0,
      0.0
    }
  }
} ;

const ap_input Autopilot_rtZap_input = { { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }
};

real_T look1_binlxpw(real_T u0, const real_T bp0[], const real_T table[], uint32_T maxIndex)
{
  real_T frac;
  uint32_T iRght;
  uint32_T iLeft;
  uint32_T bpIdx;
  if (u0 <= bp0[0U]) {
    iLeft = 0U;
    frac = (u0 - bp0[0U]) / (bp0[1U] - bp0[0U]);
  } else if (u0 < bp0[maxIndex]) {
    bpIdx = maxIndex >> 1U;
    iLeft = 0U;
    iRght = maxIndex;
    while (iRght - iLeft > 1U) {
      if (u0 < bp0[bpIdx]) {
        iRght = bpIdx;
      } else {
        iLeft = bpIdx;
      }

      bpIdx = (iRght + iLeft) >> 1U;
    }

    frac = (u0 - bp0[iLeft]) / (bp0[iLeft + 1U] - bp0[iLeft]);
  } else {
    iLeft = maxIndex - 1U;
    frac = (u0 - bp0[maxIndex - 1U]) / (bp0[maxIndex] - bp0[maxIndex - 1U]);
  }

  return (table[iLeft + 1U] - table[iLeft]) * frac + table[iLeft];
}

void AutopilotModelClass::Autopilot_Chart_Init(rtDW_Chart_Autopilot_T *localDW)
{
  localDW->is_active_c4_Autopilot = 0U;
  localDW->is_c4_Autopilot = Autopilot_IN_NO_ACTIVE_CHILD;
}

void AutopilotModelClass::Autopilot_Chart(real_T rtu_right, real_T rtu_left, real_T rtu_use_short_path, real_T *rty_out,
  rtDW_Chart_Autopilot_T *localDW)
{
  real_T tmp;
  real_T tmp_0;
  if (localDW->is_active_c4_Autopilot == 0U) {
    localDW->is_active_c4_Autopilot = 1U;
    localDW->is_c4_Autopilot = Autopilot_IN_any;
    if (std::abs(rtu_left) < std::abs(rtu_right)) {
      *rty_out = rtu_left;
    } else {
      *rty_out = rtu_right;
    }
  } else {
    switch (localDW->is_c4_Autopilot) {
     case Autopilot_IN_any:
      tmp = std::abs(rtu_right);
      tmp_0 = std::abs(rtu_left);
      if ((rtu_use_short_path == 0.0) && (tmp < tmp_0) && (tmp >= 10.0) && (tmp <= 20.0)) {
        localDW->is_c4_Autopilot = Autopilot_IN_right;
        *rty_out = rtu_right;
      } else if ((rtu_use_short_path == 0.0) && (tmp_0 < tmp) && (tmp_0 >= 10.0) && (tmp_0 <= 20.0)) {
        localDW->is_c4_Autopilot = Autopilot_IN_left;
        *rty_out = rtu_left;
      } else if (tmp_0 < tmp) {
        *rty_out = rtu_left;
      } else {
        *rty_out = rtu_right;
      }
      break;

     case Autopilot_IN_left:
      tmp = std::abs(rtu_left);
      tmp_0 = std::abs(rtu_right);
      if ((rtu_use_short_path != 0.0) || (tmp_0 < 10.0) || (tmp < 10.0)) {
        localDW->is_c4_Autopilot = Autopilot_IN_any;
        if (tmp < tmp_0) {
          *rty_out = rtu_left;
        } else {
          *rty_out = rtu_right;
        }
      } else {
        *rty_out = rtu_left;
      }
      break;

     default:
      tmp = std::abs(rtu_left);
      tmp_0 = std::abs(rtu_right);
      if ((rtu_use_short_path != 0.0) || (tmp_0 < 10.0) || (tmp < 10.0)) {
        localDW->is_c4_Autopilot = Autopilot_IN_any;
        if (tmp < tmp_0) {
          *rty_out = rtu_left;
        } else {
          *rty_out = rtu_right;
        }
      } else {
        *rty_out = rtu_right;
      }
      break;
    }
  }
}

real_T rt_modd(real_T u0, real_T u1)
{
  real_T y;
  boolean_T yEq;
  real_T q;
  y = u0;
  if (u1 == 0.0) {
    if (u0 == 0.0) {
      y = u1;
    }
  } else if (u0 == 0.0) {
    y = 0.0 / u1;
  } else {
    y = std::fmod(u0, u1);
    yEq = (y == 0.0);
    if ((!yEq) && (u1 > std::floor(u1))) {
      q = std::abs(u0 / u1);
      yEq = (std::abs(q - std::floor(q + 0.5)) <= DBL_EPSILON * q);
    }

    if (yEq) {
      y = 0.0;
    } else {
      if ((u0 < 0.0) != (u1 < 0.0)) {
        y += u1;
      }
    }
  }

  return y;
}

void AutopilotModelClass::step()
{
  real_T x[3];
  real_T rtb_Min_g;
  real_T rtb_Tsxlo;
  real_T rtb_Mod1;
  real_T rtb_Sum3;
  real_T rtb_Mod1_aj;
  int32_T rtb_out_e;
  real_T rtb_kntoms;
  real_T rtb_IC;
  real_T rtb_kntoms_l;
  real_T rtb_IC_d;
  real_T rtb_kntoms_f;
  real_T rtb_IC_p;
  real_T rtb_Gain_h;
  real_T rtb_Sum4_n;
  real_T rtb_Sum_f;
  real_T rtb_out_h;
  real_T rtb_out_d;
  real_T rtb_out;
  real_T rtb_out_o;
  real_T rtb_out_ee;
  int32_T rtb_LAW;
  real_T rtb_Sum1_lr;
  real_T u0;
  if (Autopilot_DWork.is_active_c3_Autopilot == 0U) {
    Autopilot_DWork.is_active_c3_Autopilot = 1U;
    Autopilot_DWork.is_c3_Autopilot = Autopilot_IN_OFF;
    rtb_out_e = 0;
  } else if (Autopilot_DWork.is_c3_Autopilot == Autopilot_IN_OFF) {
    if (Autopilot_U.in.input.trigger_ap_master == 1.0) {
      Autopilot_DWork.is_c3_Autopilot = Autopilot_IN_ON;
      rtb_out_e = 1;
    } else {
      rtb_out_e = 0;
    }
  } else {
    if ((Autopilot_U.in.input.trigger_ap_master == 1.0) || (Autopilot_U.in.input.trigger_ap_off == 1.0)) {
      Autopilot_DWork.is_c3_Autopilot = Autopilot_IN_OFF;
      rtb_out_e = 0;
    } else {
      rtb_out_e = 1;
    }
  }

  if (Autopilot_DWork.is_active_c1_Autopilot == 0U) {
    Autopilot_DWork.is_active_c1_Autopilot = 1U;
    Autopilot_DWork.is_c1_Autopilot = Autopilot_IN_ALT_HOLD;
    rtb_LAW = 1;
  } else {
    switch (Autopilot_DWork.is_c1_Autopilot) {
     case Autopilot_IN_ALT_ACQ:
      rtb_Min_g = std::abs(Autopilot_U.in.data.ap_H_c_ft - Autopilot_U.in.data.H_ind_ft);
      if (rtb_Min_g <= 20.0) {
        Autopilot_DWork.is_c1_Autopilot = Autopilot_IN_ALT_HOLD;
        rtb_LAW = 1;
      } else if (rtb_Min_g > 1000.0) {
        Autopilot_DWork.is_c1_Autopilot = Autopilot_IN_SPD_MACH;
        rtb_LAW = 3;
      } else {
        rtb_LAW = 2;
      }
      break;

     case Autopilot_IN_ALT_HOLD:
      if (Autopilot_U.in.input.trigger_appr == 1.0) {
        Autopilot_DWork.is_c1_Autopilot = Autopilot_IN_GS;
        rtb_LAW = 5;
      } else if (std::abs(Autopilot_U.in.data.ap_H_c_ft - Autopilot_U.in.data.H_ind_ft) > 250.0) {
        Autopilot_DWork.is_c1_Autopilot = Autopilot_IN_SPD_MACH;
        rtb_LAW = 3;
      } else {
        rtb_LAW = 1;
      }
      break;

     case Autopilot_IN_GS:
      if (Autopilot_U.in.input.trigger_appr == 1.0) {
        Autopilot_DWork.is_c1_Autopilot = Autopilot_IN_ALT_HOLD;
        rtb_LAW = 1;
      } else {
        rtb_LAW = 5;
      }
      break;

     case Autopilot_IN_SPD_MACH:
      if (std::abs(Autopilot_U.in.data.ap_H_c_ft - Autopilot_U.in.data.H_ind_ft) <= std::abs
          (Autopilot_U.in.data.H_dot_ft_min) / 8.0) {
        Autopilot_DWork.is_c1_Autopilot = Autopilot_IN_ALT_ACQ;
        rtb_LAW = 2;
      } else if (Autopilot_U.in.input.trigger_vs_mode == 1.0) {
        Autopilot_DWork.is_c1_Autopilot = Autopilot_IN_VS;
        rtb_LAW = 4;
      } else {
        rtb_LAW = 3;
      }
      break;

     default:
      if (std::abs(Autopilot_U.in.data.ap_H_c_ft - Autopilot_U.in.data.H_ind_ft) <= std::abs
          (Autopilot_U.in.data.H_dot_ft_min) / 8.0) {
        Autopilot_DWork.is_c1_Autopilot = Autopilot_IN_ALT_ACQ;
        rtb_LAW = 2;
      } else if (Autopilot_U.in.input.trigger_alt_mode != 0.0) {
        Autopilot_DWork.is_c1_Autopilot = Autopilot_IN_SPD_MACH;
        rtb_LAW = 3;
      } else {
        rtb_LAW = 4;
      }
      break;
    }
  }

  if (Autopilot_DWork.is_active_c2_Autopilot == 0U) {
    Autopilot_DWork.is_active_c2_Autopilot = 1U;
    Autopilot_DWork.is_c2_Autopilot = Autopilot_IN_SELECTED;
    Autopilot_B.LAW = 1.0;
  } else {
    switch (Autopilot_DWork.is_c2_Autopilot) {
     case Autopilot_IN_LOC:
      if (Autopilot_U.in.input.trigger_hdg_mode == 1.0) {
        Autopilot_DWork.is_LOC = Autopilot_IN_NO_ACTIVE_CHILD_a;
        Autopilot_DWork.is_c2_Autopilot = Autopilot_IN_SELECTED;
        Autopilot_B.LAW = 1.0;
      } else if (Autopilot_U.in.input.trigger_hdg_mode == 2.0) {
        Autopilot_DWork.is_LOC = Autopilot_IN_NO_ACTIVE_CHILD_a;
        Autopilot_DWork.is_c2_Autopilot = Autopilot_IN_MANAGED;
        Autopilot_B.LAW = 3.0;
      } else {
        switch (Autopilot_DWork.is_LOC) {
         case Autopilot_IN_COND:
          if (Autopilot_U.in.time.simulation_time - Autopilot_DWork.loc_trk_time >= 10.0) {
            Autopilot_DWork.is_LOC = Autopilot_IN_LOC_TRK;
            Autopilot_B.LAW = 5.0;
          } else {
            if (std::abs(Autopilot_U.in.data.nav_radial_error_deg) >= 0.16) {
              Autopilot_DWork.is_LOC = Autopilot_IN_LOC_CPT;
              Autopilot_B.LAW = 4.0;
            }
          }
          break;

         case Autopilot_IN_LOC_CPT:
          if (std::abs(Autopilot_U.in.data.nav_radial_error_deg) < 0.16) {
            Autopilot_DWork.loc_trk_time = Autopilot_U.in.time.simulation_time;
            Autopilot_DWork.is_LOC = Autopilot_IN_COND;
          } else {
            Autopilot_B.LAW = 4.0;
          }
          break;

         default:
          Autopilot_B.LAW = 5.0;
          break;
        }
      }
      break;

     case Autopilot_IN_MANAGED:
      if (Autopilot_U.in.input.trigger_loc == 1.0) {
        Autopilot_DWork.is_c2_Autopilot = Autopilot_IN_LOC;
        Autopilot_DWork.is_LOC = Autopilot_IN_LOC_CPT;
        Autopilot_B.LAW = 4.0;
      } else if (Autopilot_U.in.input.trigger_hdg_mode == 1.0) {
        Autopilot_DWork.is_c2_Autopilot = Autopilot_IN_SELECTED;
        Autopilot_B.LAW = 1.0;
      } else {
        Autopilot_B.LAW = 3.0;
      }
      break;

     default:
      if (Autopilot_U.in.input.trigger_hdg_mode == 2.0) {
        Autopilot_DWork.is_c2_Autopilot = Autopilot_IN_MANAGED;
        Autopilot_B.LAW = 3.0;
      } else if (Autopilot_U.in.input.trigger_loc == 1.0) {
        Autopilot_DWork.is_c2_Autopilot = Autopilot_IN_LOC;
        Autopilot_DWork.is_LOC = Autopilot_IN_LOC_CPT;
        Autopilot_B.LAW = 4.0;
      } else {
        Autopilot_B.LAW = 1.0;
      }
      break;
    }
  }

  rtb_Mod1 = rt_modd((Autopilot_U.in.data.ap_Psi_c_deg - (Autopilot_U.in.data.Psi_magnetic_deg +
    Autopilot_P.Constant3_Value)) + Autopilot_P.Constant3_Value, Autopilot_P.Constant3_Value);
  Autopilot_Chart(rtb_Mod1, Autopilot_P.Gain_Gain_b * rt_modd(Autopilot_P.Constant3_Value - rtb_Mod1,
    Autopilot_P.Constant3_Value), Autopilot_P.Constant_Value_e, &rtb_out_d, &Autopilot_DWork.sf_Chart_e2);
  rtb_Mod1 = rt_modd((Autopilot_U.in.data.ap_Psi_c_deg - (Autopilot_U.in.data.Psi_magnetic_track_deg +
    Autopilot_P.Constant3_Value_g)) + Autopilot_P.Constant3_Value_g, Autopilot_P.Constant3_Value_g);
  Autopilot_Chart(rtb_Mod1, Autopilot_P.Gain_Gain_a * rt_modd(Autopilot_P.Constant3_Value_g - rtb_Mod1,
    Autopilot_P.Constant3_Value_g), Autopilot_P.Constant_Value_k, &rtb_out, &Autopilot_DWork.sf_Chart_f);
  rtb_out_ee = Autopilot_P.Gain_Gain_aa * Autopilot_U.in.data.flight_guidance_xtk_nmi;
  if (rtb_out_ee > Autopilot_P.Saturation_UpperSat_k) {
    rtb_out_ee = Autopilot_P.Saturation_UpperSat_k;
  } else {
    if (rtb_out_ee < Autopilot_P.Saturation_LowerSat_g) {
      rtb_out_ee = Autopilot_P.Saturation_LowerSat_g;
    }
  }

  rtb_Mod1 = rt_modd((rt_modd((Autopilot_P.Gain2_Gain_a * Autopilot_U.in.data.flight_guidance_tae_deg + rtb_out_ee) *
    Autopilot_P.Gain1_Gain_p + (Autopilot_U.in.data.Psi_magnetic_track_deg + Autopilot_P.Constant_Value_a),
    Autopilot_P.Constant_Value_a) - (Autopilot_U.in.data.Psi_magnetic_track_deg + Autopilot_P.Constant3_Value_o)) +
                     Autopilot_P.Constant3_Value_o, Autopilot_P.Constant3_Value_o);
  Autopilot_Chart(rtb_Mod1, Autopilot_P.Gain_Gain_l * rt_modd(Autopilot_P.Constant3_Value_o - rtb_Mod1,
    Autopilot_P.Constant3_Value_o), Autopilot_P.Constant_Value_g, &rtb_out_o, &Autopilot_DWork.sf_Chart_e);
  rtb_out_ee = std::sin(Autopilot_P.Gain1_Gain_o * Autopilot_U.in.data.nav_radial_error_deg) *
    Autopilot_U.in.data.nav_dme_nmi * Autopilot_P.Gain2_Gain_p;
  if (rtb_out_ee > Autopilot_P.Saturation1_UpperSat) {
    rtb_out_ee = Autopilot_P.Saturation1_UpperSat;
  } else {
    if (rtb_out_ee < Autopilot_P.Saturation1_LowerSat) {
      rtb_out_ee = Autopilot_P.Saturation1_LowerSat;
    }
  }

  rtb_Mod1 = rt_modd((rt_modd((((Autopilot_U.in.data.nav_radial_error_deg + Autopilot_U.in.data.nav_loc_deg) -
    Autopilot_U.in.data.Psi_magnetic_track_deg) + rtb_out_ee) * Autopilot_P.Gain3_Gain +
    (Autopilot_U.in.data.Psi_magnetic_track_deg + Autopilot_P.Constant_Value_l), Autopilot_P.Constant_Value_l) -
                      (Autopilot_U.in.data.Psi_magnetic_track_deg + Autopilot_P.Constant3_Value_h)) +
                     Autopilot_P.Constant3_Value_h, Autopilot_P.Constant3_Value_h);
  Autopilot_Chart(rtb_Mod1, Autopilot_P.Gain_Gain_l2 * rt_modd(Autopilot_P.Constant3_Value_h - rtb_Mod1,
    Autopilot_P.Constant3_Value_h), Autopilot_P.Constant_Value_b, &rtb_out_ee, &Autopilot_DWork.sf_Chart_a);
  rtb_Mod1 = Autopilot_P.DiscreteDerivativeVariableTs_Gain * Autopilot_U.in.data.nav_radial_error_deg;
  rtb_Sum3 = (rtb_Mod1 - Autopilot_DWork.Delay_DSTATE) / Autopilot_U.in.time.dt * Autopilot_P.Gain3_Gain_o +
    Autopilot_U.in.data.nav_radial_error_deg;
  rtb_out_h = Autopilot_U.in.time.dt * Autopilot_P.LagFilter_C1;
  rtb_Min_g = rtb_out_h + Autopilot_P.Constant_Value_bb;
  Autopilot_DWork.Delay1_DSTATE = 1.0 / rtb_Min_g * (Autopilot_P.Constant_Value_bb - rtb_out_h) *
    Autopilot_DWork.Delay1_DSTATE + (rtb_Sum3 + Autopilot_DWork.Delay_DSTATE_b) * (rtb_out_h / rtb_Min_g);
  rtb_Mod1_aj = rt_modd((rt_modd(Autopilot_P.Gain4_Gain_f * Autopilot_DWork.Delay1_DSTATE +
    (Autopilot_U.in.data.Psi_magnetic_track_deg + Autopilot_P.Constant_Value_c), Autopilot_P.Constant_Value_c) -
    (Autopilot_U.in.data.Psi_magnetic_track_deg + Autopilot_P.Constant3_Value_b)) + Autopilot_P.Constant3_Value_b,
                        Autopilot_P.Constant3_Value_b);
  Autopilot_Chart(rtb_Mod1_aj, Autopilot_P.Gain_Gain_i3 * rt_modd(Autopilot_P.Constant3_Value_b - rtb_Mod1_aj,
    Autopilot_P.Constant3_Value_b), Autopilot_P.Constant_Value_k5, &rtb_out_h, &Autopilot_DWork.sf_Chart_p);
  if (Autopilot_P.ManualSwitch_CurrentSetting == 1) {
    rtb_Min_g = Autopilot_P.Constant_Value;
  } else {
    rtb_Min_g = Autopilot_B.LAW;
  }

  switch (static_cast<int32_T>(rtb_Min_g)) {
   case 1:
    rtb_out_h = rtb_out_d * look1_binlxpw(Autopilot_U.in.data.V_tas_kn,
      Autopilot_P.ScheduledGain_BreakpointsForDimension1, Autopilot_P.ScheduledGain_Table, 3U);
    break;

   case 2:
    rtb_out_h = rtb_out * look1_binlxpw(Autopilot_U.in.data.V_tas_kn,
      Autopilot_P.ScheduledGain_BreakpointsForDimension1_h, Autopilot_P.ScheduledGain_Table_c, 3U);
    break;

   case 3:
    rtb_out_h = rtb_out_o * look1_binlxpw(Autopilot_U.in.data.V_tas_kn,
      Autopilot_P.ScheduledGain_BreakpointsForDimension1_i, Autopilot_P.ScheduledGain_Table_m, 3U);
    break;

   case 4:
    rtb_out_h = rtb_out_ee * look1_binlxpw(Autopilot_U.in.data.V_tas_kn,
      Autopilot_P.ScheduledGain_BreakpointsForDimension1_g, Autopilot_P.ScheduledGain_Table_i, 3U);
    break;

   default:
    rtb_out_h *= look1_binlxpw(Autopilot_U.in.data.V_tas_kn, Autopilot_P.ScheduledGain_BreakpointsForDimension1_f,
      Autopilot_P.ScheduledGain_Table_o, 3U);
    break;
  }

  rtb_out_d = look1_binlxpw(Autopilot_U.in.data.V_tas_kn, Autopilot_P.ROLLLIM1_bp01Data, Autopilot_P.ROLLLIM1_tableData,
    4U);
  if (rtb_out_h > rtb_out_d) {
    rtb_out_h = rtb_out_d;
  } else {
    rtb_out_d *= Autopilot_P.Gain1_Gain;
    if (rtb_out_h < rtb_out_d) {
      rtb_out_h = rtb_out_d;
    }
  }

  rtb_out_d = Autopilot_P.Gain_Gain_by * Autopilot_U.in.data.Phi_deg;
  if (rtb_out_e == 0) {
    Autopilot_DWork.icLoad = 1U;
  }

  if (Autopilot_DWork.icLoad != 0) {
    Autopilot_DWork.Delay_DSTATE_i = rtb_out_d;
  }

  rtb_out_ee = rtb_out_h - Autopilot_DWork.Delay_DSTATE_i;
  rtb_kntoms_l = Autopilot_P.Constant2_Value * Autopilot_U.in.time.dt;
  if (rtb_out_ee < rtb_kntoms_l) {
    rtb_kntoms_l = rtb_out_ee;
  }

  rtb_out_ee = Autopilot_P.Gain1_Gain_me * Autopilot_P.Constant2_Value * Autopilot_U.in.time.dt;
  if (rtb_kntoms_l > rtb_out_ee) {
    rtb_out_ee = rtb_kntoms_l;
  }

  Autopilot_DWork.Delay_DSTATE_i += rtb_out_ee;
  rtb_out = Autopilot_U.in.time.dt * Autopilot_P.LagFilter_C1_a;
  rtb_Min_g = rtb_out + Autopilot_P.Constant_Value_f;
  Autopilot_DWork.Delay1_DSTATE_b = 1.0 / rtb_Min_g * (Autopilot_P.Constant_Value_f - rtb_out) *
    Autopilot_DWork.Delay1_DSTATE_b + (Autopilot_DWork.Delay_DSTATE_i + Autopilot_DWork.Delay_DSTATE_iq) * (rtb_out /
    rtb_Min_g);
  rtb_out_ee = static_cast<real_T>(rtb_out_e) - Autopilot_DWork.Delay_DSTATE_m;
  rtb_kntoms_l = Autopilot_P.RateLimiterVariableTs_up * Autopilot_U.in.time.dt;
  if (rtb_out_ee < rtb_kntoms_l) {
    rtb_kntoms_l = rtb_out_ee;
  }

  rtb_out_ee = Autopilot_U.in.time.dt * Autopilot_P.RateLimiterVariableTs_lo;
  if (rtb_kntoms_l > rtb_out_ee) {
    rtb_out_ee = rtb_kntoms_l;
  }

  Autopilot_DWork.Delay_DSTATE_m += rtb_out_ee;
  if (Autopilot_DWork.Delay_DSTATE_m > Autopilot_P.Saturation_UpperSat_h) {
    rtb_out = Autopilot_P.Saturation_UpperSat_h;
  } else if (Autopilot_DWork.Delay_DSTATE_m < Autopilot_P.Saturation_LowerSat_h) {
    rtb_out = Autopilot_P.Saturation_LowerSat_h;
  } else {
    rtb_out = Autopilot_DWork.Delay_DSTATE_m;
  }

  Autopilot_Y.out.output.autopilot.Phi_c_deg = (Autopilot_P.Constant_Value_lk - rtb_out) * rtb_out_d +
    Autopilot_DWork.Delay1_DSTATE_b * rtb_out;
  rtb_kntoms = Autopilot_P.kntoms_Gain * Autopilot_U.in.data.V_tas_kn;
  if (Autopilot_DWork.IC_FirstOutputTime) {
    Autopilot_DWork.IC_FirstOutputTime = false;
    rtb_IC = Autopilot_P.IC_Value;
  } else {
    rtb_IC = rtb_kntoms;
  }

  rtb_kntoms_l = Autopilot_P.kntoms_Gain_e * Autopilot_U.in.data.V_tas_kn;
  if (Autopilot_DWork.IC_FirstOutputTime_f) {
    Autopilot_DWork.IC_FirstOutputTime_f = false;
    rtb_IC_d = Autopilot_P.IC_Value_h;
  } else {
    rtb_IC_d = rtb_kntoms_l;
  }

  rtb_out_d = Autopilot_P.DiscreteDerivativeVariableTs_Gain_p * Autopilot_U.in.data.V_ias_kn;
  rtb_out = (rtb_out_d - Autopilot_DWork.Delay_DSTATE_bk) / Autopilot_U.in.time.dt * Autopilot_P.Gain3_Gain_a +
    Autopilot_U.in.data.V_ias_kn;
  rtb_Min_g = Autopilot_U.in.time.dt * Autopilot_P.LagFilter1_C1;
  rtb_Tsxlo = rtb_Min_g + Autopilot_P.Constant_Value_e3;
  Autopilot_DWork.Delay1_DSTATE_c = 1.0 / rtb_Tsxlo * (Autopilot_P.Constant_Value_e3 - rtb_Min_g) *
    Autopilot_DWork.Delay1_DSTATE_c + (rtb_out + Autopilot_DWork.Delay_DSTATE_o) * (rtb_Min_g / rtb_Tsxlo);
  rtb_out_ee = Autopilot_DWork.Delay1_DSTATE_c - Autopilot_U.in.data.ap_V_c_kn;
  if (rtb_out_ee > Autopilot_P.Saturation_UpperSat_l) {
    rtb_out_ee = Autopilot_P.Saturation_UpperSat_l;
  } else {
    if (rtb_out_ee < Autopilot_P.Saturation_LowerSat_e) {
      rtb_out_ee = Autopilot_P.Saturation_LowerSat_e;
    }
  }

  rtb_Tsxlo = Autopilot_P.Gain1_Gain_a * rtb_out_ee;
  rtb_Min_g = Autopilot_P.Gain1_Gain_i * Autopilot_U.in.data.alpha_deg;
  rtb_Sum_f = Autopilot_U.in.data.bz_m_s2 * std::sin(rtb_Min_g);
  rtb_Min_g = std::cos(rtb_Min_g);
  rtb_Min_g *= Autopilot_U.in.data.bx_m_s2;
  rtb_Sum_f = (rtb_Sum_f + rtb_Min_g) * Autopilot_P.Gain_Gain_e * Autopilot_P.Gain_Gain_o;
  rtb_out_ee = Autopilot_P.Gain1_Gain_b * std::abs(Autopilot_P.Constant_Value_h) + std::abs(rtb_Sum_f);
  if (rtb_out_ee <= Autopilot_P.Constant1_Value_e) {
    rtb_out_ee = Autopilot_P.Constant1_Value_e;
  }

  rtb_Min_g = std::abs(rtb_Tsxlo);
  if (rtb_Min_g < rtb_out_ee) {
    rtb_out_ee = rtb_Min_g;
  }

  if (rtb_Sum_f < 0.0) {
    rtb_Sum_f = -1.0;
  } else {
    if (rtb_Sum_f > 0.0) {
      rtb_Sum_f = 1.0;
    }
  }

  rtb_Sum1_lr = rtb_Tsxlo - rtb_out_ee * rtb_Sum_f * Autopilot_P.Gain_Gain_fk;
  rtb_Sum_f = Autopilot_P.kntoms_Gain_c * Autopilot_U.in.data.V_tas_kn;
  rtb_out_ee = std::sin((Autopilot_P.Gain2_Gain_d * Autopilot_U.in.data.Theta_deg - std::cos(Autopilot_P.Gain1_Gain_d *
    Autopilot_U.in.data.Phi_deg) * Autopilot_U.in.data.alpha_deg) * Autopilot_P.Gain1_Gain_g) * rtb_Sum_f *
    Autopilot_P.msftmin_Gain_b;
  if (Autopilot_DWork.IC_FirstOutputTime_a) {
    Autopilot_DWork.IC_FirstOutputTime_a = false;
    rtb_Sum_f = Autopilot_P.IC_Value_k;
  }

  rtb_out_ee = (Autopilot_P.Constant_Value_ea - rtb_out_ee) * Autopilot_P.ftmintoms_Gain_d / rtb_Sum_f;
  rtb_kntoms_f = Autopilot_P.kntoms_Gain_p * Autopilot_U.in.data.V_tas_kn;
  if (Autopilot_DWork.IC_FirstOutputTime_i) {
    Autopilot_DWork.IC_FirstOutputTime_i = false;
    rtb_IC_p = Autopilot_P.IC_Value_n;
  } else {
    rtb_IC_p = rtb_kntoms_f;
  }

  rtb_out_o = Autopilot_P.DiscreteDerivativeVariableTs_Gain_o * Autopilot_U.in.data.nav_gs_error_deg;
  rtb_Mod1_aj = (rtb_out_o - Autopilot_DWork.Delay_DSTATE_n) / Autopilot_U.in.time.dt * Autopilot_P.Gain3_Gain_ad +
    Autopilot_P.Gain1_Gain_kq * Autopilot_U.in.data.nav_gs_error_deg;
  rtb_Sum_f = Autopilot_U.in.time.dt * Autopilot_P.LagFilter_C1_m;
  rtb_Min_g = rtb_Sum_f + Autopilot_P.Constant_Value_br;
  Autopilot_DWork.Delay1_DSTATE_l = 1.0 / rtb_Min_g * (Autopilot_P.Constant_Value_br - rtb_Sum_f) *
    Autopilot_DWork.Delay1_DSTATE_l + (rtb_Mod1_aj + Autopilot_DWork.Delay_DSTATE_j) * (rtb_Sum_f / rtb_Min_g);
  rtb_Gain_h = Autopilot_P.DiscreteDerivativeVariableTs_Gain_j * Autopilot_U.in.data.V_ias_kn;
  rtb_Sum4_n = (rtb_Gain_h - Autopilot_DWork.Delay_DSTATE_g) / Autopilot_U.in.time.dt * Autopilot_P.Gain3_Gain_j +
    Autopilot_U.in.data.V_ias_kn;
  rtb_Sum_f = Autopilot_U.in.time.dt * Autopilot_P.LagFilter1_C1_e;
  rtb_Min_g = rtb_Sum_f + Autopilot_P.Constant_Value_ax;
  Autopilot_DWork.Delay1_DSTATE_h = 1.0 / rtb_Min_g * (Autopilot_P.Constant_Value_ax - rtb_Sum_f) *
    Autopilot_DWork.Delay1_DSTATE_h + (rtb_Sum4_n + Autopilot_DWork.Delay_DSTATE_e) * (rtb_Sum_f / rtb_Min_g);
  rtb_Min_g = Autopilot_DWork.Delay1_DSTATE_h - Autopilot_U.in.data.V_c_srs_kn;
  if (rtb_Min_g > Autopilot_P.Saturation_UpperSat_f) {
    rtb_Min_g = Autopilot_P.Saturation_UpperSat_f;
  } else {
    if (rtb_Min_g < Autopilot_P.Saturation_LowerSat_b) {
      rtb_Min_g = Autopilot_P.Saturation_LowerSat_b;
    }
  }

  rtb_Sum_f = Autopilot_P.Gain1_Gain_f * rtb_Min_g;
  rtb_Tsxlo = Autopilot_P.Gain1_Gain_do * Autopilot_U.in.data.alpha_deg;
  rtb_Min_g = Autopilot_U.in.data.bz_m_s2 * std::sin(rtb_Tsxlo);
  rtb_Tsxlo = std::cos(rtb_Tsxlo);
  rtb_Tsxlo *= Autopilot_U.in.data.bx_m_s2;
  rtb_Tsxlo = (rtb_Min_g + rtb_Tsxlo) * Autopilot_P.Gain_Gain_h * Autopilot_P.Gain_Gain_iq;
  rtb_Min_g = Autopilot_P.Gain1_Gain_ig * std::abs(Autopilot_P.Constant_Value_he) + std::abs(rtb_Tsxlo);
  if (rtb_Min_g <= Autopilot_P.Constant1_Value_o) {
    rtb_Min_g = Autopilot_P.Constant1_Value_o;
  }

  u0 = std::abs(rtb_Sum_f);
  if (u0 < rtb_Min_g) {
    rtb_Min_g = u0;
  }

  if (rtb_Tsxlo < 0.0) {
    rtb_Tsxlo = -1.0;
  } else {
    if (rtb_Tsxlo > 0.0) {
      rtb_Tsxlo = 1.0;
    }
  }

  rtb_Tsxlo = rtb_Sum_f - rtb_Min_g * rtb_Tsxlo * Autopilot_P.Gain_Gain_bs;
  rtb_Sum_f = Autopilot_P.kntoms_Gain_d * Autopilot_U.in.data.V_tas_kn;
  rtb_Min_g = std::sin((Autopilot_P.Gain2_Gain_f * Autopilot_U.in.data.Theta_deg - std::cos(Autopilot_P.Gain1_Gain_ad *
    Autopilot_U.in.data.Phi_deg) * Autopilot_U.in.data.alpha_deg) * Autopilot_P.Gain1_Gain_a3) * rtb_Sum_f;
  if (Autopilot_DWork.IC_FirstOutputTime_c) {
    Autopilot_DWork.IC_FirstOutputTime_c = false;
    rtb_Sum_f = Autopilot_P.IC_Value_d;
  }

  rtb_Min_g = (Autopilot_P.Constant_Value_i - Autopilot_P.msftmin_Gain_d * rtb_Min_g) * Autopilot_P.ftmintoms_Gain_f /
    rtb_Sum_f;
  x[0] = Autopilot_P.Constant1_Value_i - Autopilot_P.Gain1_Gain_ji * Autopilot_U.in.data.Theta_deg;
  x[1] = rtb_Tsxlo;
  if (rtb_Min_g > 1.0) {
    rtb_Min_g = 1.0;
  } else {
    if (rtb_Min_g < -1.0) {
      rtb_Min_g = -1.0;
    }
  }

  x[2] = Autopilot_P.Gain_Gain_d5 * std::asin(rtb_Min_g) * Autopilot_P.Gain_Gain_a5;
  if (Autopilot_P.ManualSwitch_CurrentSetting_h == 1) {
    rtb_Min_g = Autopilot_P.Constant_Value_o;
  } else {
    rtb_Min_g = rtb_LAW;
  }

  switch (static_cast<int32_T>(rtb_Min_g)) {
   case 1:
    rtb_out_ee = (Autopilot_U.in.data.ap_H_c_ft - Autopilot_U.in.data.H_ind_ft) * Autopilot_P.Gain_Gain_ml;
    if (rtb_out_ee > Autopilot_P.Saturation_UpperSat) {
      rtb_out_ee = Autopilot_P.Saturation_UpperSat;
    } else {
      if (rtb_out_ee < Autopilot_P.Saturation_LowerSat) {
        rtb_out_ee = Autopilot_P.Saturation_LowerSat;
      }
    }

    rtb_out_ee = (rtb_out_ee - std::sin((Autopilot_P.Gain2_Gain_kp * Autopilot_U.in.data.Theta_deg - std::cos
      (Autopilot_P.Gain1_Gain_l * Autopilot_U.in.data.Phi_deg) * Autopilot_U.in.data.alpha_deg) *
      Autopilot_P.Gain1_Gain_kz) * rtb_kntoms * Autopilot_P.msftmin_Gain_p) * Autopilot_P.ftmintoms_Gain_k / rtb_IC;
    if (rtb_out_ee > 1.0) {
      rtb_out_ee = 1.0;
    } else {
      if (rtb_out_ee < -1.0) {
        rtb_out_ee = -1.0;
      }
    }

    rtb_Sum_f = Autopilot_P.Gain_Gain_f * std::asin(rtb_out_ee) * Autopilot_P.Gain_Gain_p1;
    break;

   case 2:
    rtb_out_ee = Autopilot_U.in.data.ap_H_c_ft - Autopilot_U.in.data.H_ind_ft;
    if (rtb_out_ee < 0.0) {
      rtb_Min_g = -1.0;
    } else if (rtb_out_ee > 0.0) {
      rtb_Min_g = 1.0;
    } else {
      rtb_Min_g = rtb_out_ee;
    }

    rtb_out_ee = ((Autopilot_P.Constant_Value_j * rtb_Min_g + rtb_out_ee) * Autopilot_P.Gain_Gain_m - std::sin
                  ((Autopilot_P.Gain2_Gain_k1 * Autopilot_U.in.data.Theta_deg - std::cos(Autopilot_P.Gain1_Gain_c *
      Autopilot_U.in.data.Phi_deg) * Autopilot_U.in.data.alpha_deg) * Autopilot_P.Gain1_Gain_k) * rtb_kntoms_l *
                  Autopilot_P.msftmin_Gain_j) * Autopilot_P.ftmintoms_Gain_n / rtb_IC_d;
    if (rtb_out_ee > 1.0) {
      rtb_out_ee = 1.0;
    } else {
      if (rtb_out_ee < -1.0) {
        rtb_out_ee = -1.0;
      }
    }

    rtb_Sum_f = Autopilot_P.Gain_Gain_i * std::asin(rtb_out_ee) * Autopilot_P.Gain_Gain_p;
    break;

   case 3:
    if (Autopilot_U.in.data.ap_H_c_ft - Autopilot_U.in.data.H_ind_ft > Autopilot_P.Switch_Threshold) {
      if (rtb_out_ee > 1.0) {
        rtb_out_ee = 1.0;
      } else {
        if (rtb_out_ee < -1.0) {
          rtb_out_ee = -1.0;
        }
      }

      rtb_Sum_f = Autopilot_P.Gain_Gain_d * std::asin(rtb_out_ee) * Autopilot_P.Gain_Gain_fh;
      if (rtb_Sum1_lr > rtb_Sum_f) {
        rtb_Sum_f = rtb_Sum1_lr;
      }
    } else {
      if (rtb_out_ee > 1.0) {
        rtb_out_ee = 1.0;
      } else {
        if (rtb_out_ee < -1.0) {
          rtb_out_ee = -1.0;
        }
      }

      rtb_Sum_f = Autopilot_P.Gain_Gain_d * std::asin(rtb_out_ee) * Autopilot_P.Gain_Gain_fh;
      if (rtb_Sum1_lr < rtb_Sum_f) {
        rtb_Sum_f = rtb_Sum1_lr;
      }
    }
    break;

   case 4:
    rtb_out_ee = (Autopilot_U.in.data.ap_H_dot_c_ft_min - std::sin((Autopilot_P.Gain2_Gain_k *
      Autopilot_U.in.data.Theta_deg - std::cos(Autopilot_P.Gain1_Gain_m3 * Autopilot_U.in.data.Phi_deg) *
      Autopilot_U.in.data.alpha_deg) * Autopilot_P.Gain1_Gain_j) * rtb_kntoms_f * Autopilot_P.msftmin_Gain) *
      Autopilot_P.ftmintoms_Gain / rtb_IC_p;
    if (rtb_out_ee > 1.0) {
      rtb_out_ee = 1.0;
    } else {
      if (rtb_out_ee < -1.0) {
        rtb_out_ee = -1.0;
      }
    }

    rtb_Sum_f = Autopilot_P.Gain_Gain_c * std::asin(rtb_out_ee) * Autopilot_P.Gain_Gain_n;
    break;

   case 5:
    rtb_Sum_f = Autopilot_P.Gain4_Gain * Autopilot_DWork.Delay1_DSTATE_l;
    break;

   case 6:
    if (x[0] < rtb_Tsxlo) {
      if (rtb_Tsxlo < x[2]) {
        rtb_LAW = 1;
      } else if (x[0] < x[2]) {
        rtb_LAW = 2;
      } else {
        rtb_LAW = 0;
      }
    } else if (x[0] < x[2]) {
      rtb_LAW = 0;
    } else if (rtb_Tsxlo < x[2]) {
      rtb_LAW = 2;
    } else {
      rtb_LAW = 1;
    }

    rtb_Sum_f = x[rtb_LAW];
    break;

   default:
    rtb_Sum_f = (Autopilot_U.in.data.ap_FPA_c_deg - (Autopilot_P.Gain2_Gain * Autopilot_U.in.data.Theta_deg - std::cos
      (Autopilot_P.Gain1_Gain_m * Autopilot_U.in.data.Phi_deg) * Autopilot_U.in.data.alpha_deg)) * Autopilot_P.Gain_Gain;
    break;
  }

  rtb_IC_d = Autopilot_P.Gain_Gain_nw * Autopilot_U.in.data.Theta_deg;
  rtb_Sum_f += rtb_IC_d;
  if (rtb_Sum_f > Autopilot_P.Constant1_Value) {
    rtb_Sum_f = Autopilot_P.Constant1_Value;
  } else {
    rtb_out_ee = Autopilot_P.Gain1_Gain_kt * Autopilot_P.Constant1_Value;
    if (rtb_Sum_f < rtb_out_ee) {
      rtb_Sum_f = rtb_out_ee;
    }
  }

  if (rtb_out_e == 0) {
    Autopilot_DWork.icLoad_d = 1U;
  }

  if (Autopilot_DWork.icLoad_d != 0) {
    Autopilot_DWork.Delay_DSTATE_jx = rtb_IC_d;
  }

  rtb_out_ee = rtb_Sum_f - Autopilot_DWork.Delay_DSTATE_jx;
  rtb_kntoms_l = Autopilot_P.Constant2_Value_e * Autopilot_U.in.time.dt;
  if (rtb_out_ee < rtb_kntoms_l) {
    rtb_kntoms_l = rtb_out_ee;
  }

  rtb_out_ee = Autopilot_P.Gain1_Gain_dw * Autopilot_P.Constant2_Value_e * Autopilot_U.in.time.dt;
  if (rtb_kntoms_l > rtb_out_ee) {
    rtb_out_ee = rtb_kntoms_l;
  }

  Autopilot_DWork.Delay_DSTATE_jx += rtb_out_ee;
  rtb_Min_g = Autopilot_U.in.time.dt * Autopilot_P.LagFilter_C1_ac;
  rtb_Tsxlo = rtb_Min_g + Autopilot_P.Constant_Value_hy;
  Autopilot_DWork.Delay1_DSTATE_m = 1.0 / rtb_Tsxlo * (Autopilot_P.Constant_Value_hy - rtb_Min_g) *
    Autopilot_DWork.Delay1_DSTATE_m + (Autopilot_DWork.Delay_DSTATE_jx + Autopilot_DWork.Delay_DSTATE_d) * (rtb_Min_g /
    rtb_Tsxlo);
  rtb_out_ee = static_cast<real_T>(rtb_out_e) - Autopilot_DWork.Delay_DSTATE_c;
  rtb_kntoms_l = Autopilot_P.RateLimiterVariableTs_up_k * Autopilot_U.in.time.dt;
  if (rtb_out_ee < rtb_kntoms_l) {
    rtb_kntoms_l = rtb_out_ee;
  }

  rtb_out_ee = Autopilot_U.in.time.dt * Autopilot_P.RateLimiterVariableTs_lo_h;
  if (rtb_kntoms_l > rtb_out_ee) {
    rtb_out_ee = rtb_kntoms_l;
  }

  Autopilot_DWork.Delay_DSTATE_c += rtb_out_ee;
  if (Autopilot_DWork.Delay_DSTATE_c > Autopilot_P.Saturation_UpperSat_n) {
    rtb_Min_g = Autopilot_P.Saturation_UpperSat_n;
  } else if (Autopilot_DWork.Delay_DSTATE_c < Autopilot_P.Saturation_LowerSat_a) {
    rtb_Min_g = Autopilot_P.Saturation_LowerSat_a;
  } else {
    rtb_Min_g = Autopilot_DWork.Delay_DSTATE_c;
  }

  Autopilot_Y.out.output.autopilot.Theta_c_deg = (Autopilot_P.Constant_Value_o3 - rtb_Min_g) * rtb_IC_d +
    Autopilot_DWork.Delay1_DSTATE_m * rtb_Min_g;
  Autopilot_Y.out.time = Autopilot_U.in.time;
  Autopilot_Y.out.input = Autopilot_U.in.input;
  Autopilot_Y.out.data = Autopilot_U.in.data;
  Autopilot_Y.out.output.ap_on = rtb_out_e;
  Autopilot_Y.out.output.flight_director.Theta_c_deg = rtb_Sum_f;
  Autopilot_Y.out.output.flight_director.Phi_c_deg = rtb_out_h;
  Autopilot_Y.out.output.flight_director.Beta_c_deg = Autopilot_P.Constant_Value_f3;
  Autopilot_Y.out.output.autopilot.Beta_c_deg = Autopilot_P.Constant_Value_f3;
  Autopilot_DWork.Delay_DSTATE = rtb_Mod1;
  Autopilot_DWork.Delay_DSTATE_b = rtb_Sum3;
  Autopilot_DWork.icLoad = 0U;
  Autopilot_DWork.Delay_DSTATE_iq = Autopilot_DWork.Delay_DSTATE_i;
  Autopilot_DWork.Delay_DSTATE_bk = rtb_out_d;
  Autopilot_DWork.Delay_DSTATE_o = rtb_out;
  Autopilot_DWork.Delay_DSTATE_n = rtb_out_o;
  Autopilot_DWork.Delay_DSTATE_j = rtb_Mod1_aj;
  Autopilot_DWork.Delay_DSTATE_g = rtb_Gain_h;
  Autopilot_DWork.Delay_DSTATE_e = rtb_Sum4_n;
  Autopilot_DWork.icLoad_d = 0U;
  Autopilot_DWork.Delay_DSTATE_d = Autopilot_DWork.Delay_DSTATE_jx;
}

void AutopilotModelClass::initialize()
{
  (void) std::memset((static_cast<void *>(&Autopilot_B)), 0,
                     sizeof(BlockIO_Autopilot_T));
  (void) std::memset(static_cast<void *>(&Autopilot_DWork), 0,
                     sizeof(D_Work_Autopilot_T));
  Autopilot_U.in = Autopilot_rtZap_input;
  Autopilot_Y.out = Autopilot_rtZap_output;
  Autopilot_DWork.IC_FirstOutputTime = true;
  Autopilot_DWork.IC_FirstOutputTime_f = true;
  Autopilot_DWork.IC_FirstOutputTime_a = true;
  Autopilot_DWork.IC_FirstOutputTime_i = true;
  Autopilot_DWork.IC_FirstOutputTime_c = true;
  Autopilot_DWork.Delay_DSTATE = Autopilot_P.DiscreteDerivativeVariableTs_InitialCondition;
  Autopilot_DWork.Delay_DSTATE_b = Autopilot_P.Delay_InitialCondition;
  Autopilot_DWork.Delay1_DSTATE = Autopilot_P.Delay1_InitialCondition;
  Autopilot_DWork.icLoad = 1U;
  Autopilot_DWork.Delay_DSTATE_iq = Autopilot_P.Delay_InitialCondition_c;
  Autopilot_DWork.Delay1_DSTATE_b = Autopilot_P.Delay1_InitialCondition_e;
  Autopilot_DWork.Delay_DSTATE_m = Autopilot_P.RateLimiterVariableTs_InitialCondition;
  Autopilot_DWork.Delay_DSTATE_bk = Autopilot_P.DiscreteDerivativeVariableTs_InitialCondition_j;
  Autopilot_DWork.Delay_DSTATE_o = Autopilot_P.Delay_InitialCondition_k;
  Autopilot_DWork.Delay1_DSTATE_c = Autopilot_P.Delay1_InitialCondition_i;
  Autopilot_DWork.Delay_DSTATE_n = Autopilot_P.DiscreteDerivativeVariableTs_InitialCondition_p;
  Autopilot_DWork.Delay_DSTATE_j = Autopilot_P.Delay_InitialCondition_o;
  Autopilot_DWork.Delay1_DSTATE_l = Autopilot_P.Delay1_InitialCondition_g;
  Autopilot_DWork.Delay_DSTATE_g = Autopilot_P.DiscreteDerivativeVariableTs_InitialCondition_n;
  Autopilot_DWork.Delay_DSTATE_e = Autopilot_P.Delay_InitialCondition_ke;
  Autopilot_DWork.Delay1_DSTATE_h = Autopilot_P.Delay1_InitialCondition_n;
  Autopilot_DWork.icLoad_d = 1U;
  Autopilot_DWork.Delay_DSTATE_d = Autopilot_P.Delay_InitialCondition_f;
  Autopilot_DWork.Delay1_DSTATE_m = Autopilot_P.Delay1_InitialCondition_k;
  Autopilot_DWork.Delay_DSTATE_c = Autopilot_P.RateLimiterVariableTs_InitialCondition_i;
  Autopilot_DWork.is_active_c3_Autopilot = 0U;
  Autopilot_DWork.is_c3_Autopilot = Autopilot_IN_NO_ACTIVE_CHILD_a;
  Autopilot_DWork.is_active_c1_Autopilot = 0U;
  Autopilot_DWork.is_c1_Autopilot = Autopilot_IN_NO_ACTIVE_CHILD_a;
  Autopilot_DWork.is_LOC = Autopilot_IN_NO_ACTIVE_CHILD_a;
  Autopilot_DWork.is_active_c2_Autopilot = 0U;
  Autopilot_DWork.is_c2_Autopilot = Autopilot_IN_NO_ACTIVE_CHILD_a;
  Autopilot_Chart_Init(&Autopilot_DWork.sf_Chart_e2);
  Autopilot_Chart_Init(&Autopilot_DWork.sf_Chart_f);
  Autopilot_Chart_Init(&Autopilot_DWork.sf_Chart_e);
  Autopilot_Chart_Init(&Autopilot_DWork.sf_Chart_a);
  Autopilot_Chart_Init(&Autopilot_DWork.sf_Chart_p);
}

void AutopilotModelClass::terminate()
{
}

AutopilotModelClass::AutopilotModelClass()
{
}

AutopilotModelClass::~AutopilotModelClass()
{
}
