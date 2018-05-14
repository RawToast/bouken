module type Loop = {
    let isPlayerActive: Types.player => bool;
    let incrementPositions: Types.area => Types.area;
  };
  