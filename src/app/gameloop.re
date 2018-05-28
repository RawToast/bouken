open Types;
open Rationale;

module CreateGameLoop: ((Types.Positions) => (Types.GameLoop)) = (Pos: Types.Positions) => {
  open World;

  /* let enemyLogic = enemy => enemy; */
  type enemyInfo = {
    enemy: enemy,
    position: (int, int)
  };
/* This code should be moved to another module */
let findActiveEnemies = area => 
  area |> List.mapi((xi: int, xs: list(place)) => 
    xs |> List.mapi((yi: int, place: place) => switch place.state {
    | Enemy(e) => if(Pos.isActive(e.stats)){[ { enemy: e, position: (xi, yi) }]} else {[]}
    | _ => []
    } ) |> List.flatten
  ) |> List.flatten;
    
  let loopCost = (1. /. Pos.divisor);
  let rec continue: game => game =
    game => {
      let activeEnemies: list(enemyInfo) = game.world 
          |> World.currentLevel
          |> Option.fmap(l => findActiveEnemies(l.map))
          |> Option.default([]);

      if (Pos.isActive(game.player.stats)) {
        game;
      } else if (List.length(activeEnemies) >= 1) {
        let levelOpt = World.currentLevel(game.world);
        let activeEnemy = List.hd(activeEnemies);
        Js.Console.log(activeEnemy.enemy.name ++ " is active");        
        
        let setEnemy = (area, enemyInfo) => {
          let (x, y) = enemyInfo.position;
          Level.Area.setEnemyAt(x, y, enemyInfo.enemy, 1., area) |> Rationale.Option.ofResult
        };

        let updatedLevel = Option.bind(levelOpt, level => 
          setEnemy(level.map, activeEnemy) 
            |> Option.fmap(map => {...level, map: map })
            |> Option.fmap(l => World.updateLevel(l,game.world))
            |> Option.fmap(w => {...game, world: w})
        );
        
        Option.default(game, updatedLevel);
      } else {
        /* No one is active */
        let maybeGame = game.world 
          |> World.currentLevel
          |> Rationale.Option.fmap(l => {
            let area = Pos.incrementAll(l.map);
            let world = World.updateLevel( {...l, map: area }, game.world);
            { ...game, world: world, turn: game.turn +. loopCost }
            })
          |> Rationale.Option.fmap(g => {... g, player: {... g.player, stats: Pos.increment(g.player.stats)}});

        switch maybeGame {
        | None => game
        | Some(g) => continue(g)
        };
    };
  };
};
