open Types;

module CsvWorldBuilder: WorldCreator = {
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

  let loadWorldAsync = (initial, names) => {

  let levelNames = Js.String.split(",", names) |> Array.to_list;
   
   /* Hack, hack, hack */
    let d1 = Utils.requireAssetURI("../../public/world/Dungeon 1.csv");
    let d2 = Utils.requireAssetURI("../../public/world/Dungeon 2.csv");
    let d3 = Utils.requireAssetURI("../../public/world/Dungeon 3.csv");
    let d4 = Utils.requireAssetURI("../../public/world/Dungeon 4.csv");
  
    let d5 = Utils.requireAssetURI("../../public/world/Dungeon 5.csv");
    let c1 = Utils.requireAssetURI("../../public/world/Cave.csv");
    let s1 = Utils.requireAssetURI("../../public/world/Swamp.csv");
    let l1 = Utils.requireAssetURI("../../public/world/Labyrinth.csv");

    let lvls = [d1, d2, d3, d4, d5, c1, s1, l1];

    let prom = (name) => Js.Promise.({
      /* Fix this hack! */
      let nFileLoc = List.find(fp => String.sub(fp, 1, (String.index(fp, '.') - 1)) == name, lvls)

      Fetch.fetch(nFileLoc)
        |> then_(Fetch.Response.text)
        |> then_(text => buildLevel(name, text) |> resolve)
    }
    );

    levelNames 
      |> List.map(prom) 
      |> Array.of_list 
      |> Js.Promise.all 
      |> Js.Promise.then_(lvl => lvl |> Array.to_list |> lvs => {current: initial, levels: lvs} |> Js.Promise.resolve);
  };
};

