open Types;
let buildPlace = (str) => {
    switch str {
    | "." => { tile: GROUND, state: Empty}
    | "w" => { tile: WATER, state: Empty}
    | "#" => { tile: WALL, state: Empty}
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