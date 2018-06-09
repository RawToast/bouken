open Types;
let makeExit = score => { tile: EXIT(int_of_string((score))), state: Empty };


let makeTile = (str) => {
    switch (Js.String.charAt(0, str)) {
    | "." => { tile: GROUND, state: Empty}
    | "w" => { tile: WATER, state: Empty}
    | "#" => { tile: WALL, state: Empty}
    | "e" => makeExit(Js.String.sliceToEnd(1, str))
    | _ => { tile: WALL, state: Empty}
    };
  };

  let addEnemy = (str, place) => {
    let newEnemy = id => {id: id, name: id, stats: { health: 6, speed: 0.8, position: 0. }};
    let randId = () => Js.Math.random() |> string_of_float;
    let makeZombie = () => {id: randId(), name: "Zombie", stats: { health: 6, speed: 0.8, position: 0. }};

    switch str {
    | "Z" => { ... place, state: Enemy(makeZombie()) }
    | _ => place
    };
  };

  let buildPlace = str => { 
    let spliced = str  |> Js.String.split("|") |> Array.to_list; 
  
    switch (List.length(spliced)) {
    | 1 => spliced |> List.hd |> makeTile
    | 2 => spliced |> List.hd |> makeTile |> addEnemy(List.nth(spliced, 1))
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