open Types;
let makeExit = score => { tile: EXIT(int_of_string((score))), state: Empty };
let buildPlace = (str) => {
    switch (Js.String.charAt(0, str)) {
    | "." => { tile: GROUND, state: Empty}
    | "w" => { tile: WATER, state: Empty}
    | "#" => { tile: WALL, state: Empty}
    | "e" => makeExit(Js.String.sliceToEnd(1, str))
    | _ => { tile: WALL, state: Empty}
    };
  };


let buildArea = (str) => str 
    |> Js.String.split("\n") 
    |> Array.to_list
    |> List.map(xs =>
      xs |> Js.String.split(",") 
        |> Array.to_list
        |> List.map(Js.String.trim)
        |> List.map(buildPlace))
    |> List.filter(x => List.length(x) != 0)
    |> List.rev;