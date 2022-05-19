#ifndef __CommonDefinitions_h__
#define __CommonDefinitions_h__

#include <cmath>

enum projection{
  DIR_U=0,          // U-direction channel index
  DIR_V=1,          // V-direction channel index
  DIR_W=2,          // W-direction channel index
  DIR_XY=3,         // 2D projection on XY plane
  DIR_XZ=4,         // 2D projection on XZ plane
  DIR_YZ=5,         // 2D projection on YZ plane
  DIR_3D=6          // 3D reconstruction
};

enum pid_type{
  UNKNOWN=0,        //           <-- for backward compatibilty with data analyzed before 20 May 2022
  ALPHA=1,          // Helium-4  <-- for backward compatibilty with data analyzed before 20 May 2022
  CARBON_12=2,      // Carbon-12 <-- for backward compatibilty with data analyzed before 20 May 2022
  CARBON_14=3,      // Carbon-14 <-- for backward compatibilty with data analyzed before 20 May 2022
  C12_ALPHA=4,      //           <-- for backward compatibilty with data analyzed before 20 May 2022
  PROTON,           // Hydrogen
  CARBON_13,        // Carbon-13
  NITROGEN_15,      // Nitrogen-15
  OXYGEN_16,        // Oxygen-16
  OXYGEN_17,        // Oxygen-17
  OXYGEN_18,        // Oxygen-18
  HELIUM_4=ALPHA,   // alias
  C_12=CARBON_12,   // alias
  C_13=CARBON_13,   // alias
  C_14=CARBON_14,   // alias
  N_15=NITROGEN_15, // alias
  O_16=OXYGEN_16,   // alias
  O_17=OXYGEN_17,   // alias
  O_18=OXYGEN_18,   // alias
  PID_MIN=ALPHA,    // alias
  PID_MAX=OXYGEN_18 // alias
};

enum gas_mixture_type{
  CO2=1,            // Carbon dioxide
  GAS_MIN=CO2,
  GAS_MAX=CO2
};

#endif
