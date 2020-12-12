module CsvWorldBuilder: Types.WorldCreator = {
  open Types;

  let makeExit = score => { tile: EXIT(int_of_string((score))), state: Empty, tileEffect: NoEff, visible: false };

  let tail = Js.String.sliceToEnd(~from=1);
  let head = Js.String.charAt(0);

  let makeStairs = str => {
    let id = str |> head |> int_of_string;
    let level = str |> tail;

    let link = { id: id, level: level};
    { tile: STAIRS(link), state: Empty, tileEffect: NoEff, visible: false };
  };

  let makeTile = (str) => {
    switch (Js.String.charAt(0, str)) {
    | "." => { tile: GROUND, state: Empty, tileEffect: NoEff, visible: false }
    | ":" => { tile: ROUGH, state: Empty, tileEffect: NoEff, visible: false }
    | "w" => { tile: WATER, state: Empty, tileEffect: NoEff, visible: false }
    | "#" => { tile: WALL, state: Empty, tileEffect: NoEff, visible: false }
    | "e" => str |> tail |> makeExit
    | "/" => str |> tail |> makeStairs
    | _ => { tile: WALL, state: Empty, tileEffect: NoEff, visible: false }
    };
  };

  let createOccupier: (string, place) => place = (str, place) => {
    if(Rationale.RList.any(k => str == k, ["+", ":", "g", ";"])) {
        switch str {
        | "+" => { ... place, tileEffect: Heal(2) }
        | ":" => { ... place, tileEffect: Trap(2) }
        | ";" => { ... place, tileEffect: Snare(2.) }
        | "g" => { ... place, tileEffect: Gold(3) }
        | _ => place
        }
    } else Enemy.Enemies.addEnemy(str, place);
  };

  let buildPlace = str => { 
    let spliced = str  |> Js.String.split("|") |> Array.to_list; 

    switch (List.length(spliced)) {
    | 1 => spliced |> List.hd |> makeTile
    | 2 => spliced |> List.hd |> makeTile |> createOccupier(List.nth(spliced, 1), _)
    | _ => makeTile(".")
    };
  };

  let buildArea = (str) => str 
      |> Js.String.split("\n") 
      |> Array.to_list
      |> List.map(col =>
        col |> Js.String.split(",") 
            |> Array.to_list
            |> List.map(Js.String.trim)
            |> List.map(buildPlace))
      |> List.filter(x => List.length(x) != 0)
      |> List.rev;

  let buildLevel = (name, areaStr) => { name: name, map: buildArea(areaStr) };

  let loadWorldAsync: (string, list((string, string))) => Js.Promise.t(world) = (initial, namesAndLocations) => {
  
    let prom = ((name, location)) => Js.Promise.({
      Fetch.fetch(location)
        |> then_(Fetch.Response.text)
        |> then_(text => buildLevel(name, text) |> resolve)
    });

    namesAndLocations 
      |> List.map(ns => prom(ns)) 
      |> Array.of_list 
      |> Js.Promise.all 
      |> Js.Promise.then_(lvl => lvl |> Array.to_list |> lvs => {current: initial, levels: lvs} |> Js.Promise.resolve);
  };
};
