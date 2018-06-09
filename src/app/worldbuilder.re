open Types;
let buildPlace = (str) => {
    switch str {
    | "." => { tile: GROUND, state: Empty}
    | "w" => { tile: WATER, state: Empty}
    | "#" => { tile: WALL, state: Empty}
    | _ => { tile: WALL, state: Empty}
    };
  };