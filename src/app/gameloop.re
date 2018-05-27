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
  | Enemy(e) => [ { enemy: e, position: (xi, yi) }]
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
        Js.Console.log("Enemy is active");
        game;
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
