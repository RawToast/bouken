open Types;
let makeExit = score => { tile: EXIT(int_of_string((score))), state: Empty };

let tail = Js.String.sliceToEnd(~from=1);
let head = Js.String.charAt(0);

let makeStairs = str => {
  let id = str |> head |> int_of_string;
  let level = str |> tail;

  let link = { id: id, level: level};
  { tile: STAIRS(link), state: Empty };
};

let makeTile = (str) => {
  switch (Js.String.charAt(0, str)) {
  | "." => { tile: GROUND, state: Empty}
  | "w" => { tile: WATER, state: Empty}
  | "#" => { tile: WALL, state: Empty}
  | "e" => str |> tail |> makeExit
  | "/" => str |> tail |> makeStairs
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

let buildWorld = (current, levels) => levels 
  |> List.map(lv => { let (n, a) = lv; buildLevel(n, a)})
  |> lvls => { current: current, levels: lvls };

let loadLevel = (directory, file) =>
  Node.Fs.readFileAsUtf8Sync(directory ++ "/" ++ file) |> buildLevel(Js.String.slice(0, (Js.String.indexOf(".", file)), file));

let loadWorld = (initial, directory) => {   
  let levels = directory 
    |> Node.Fs.readdirSync 
    |> Array.to_list 
    |> List.map(file => loadLevel(directory, file));

  { current: initial, levels: levels };
};