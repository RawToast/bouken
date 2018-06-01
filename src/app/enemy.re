open Types;
open Rationale;

module CreateEnemyLoop = (Pos: Types.Positions, Places: Types.Places, World: World) => {

    let findActiveEnemies = area => 
      area |> List.mapi((xi: int, xs: list(place)) => 
        xs |> List.mapi((yi: int, place: place) => switch place.state {
        | Enemy(e) => if(Pos.isActive(e.stats)){[ { enemy: e, location: (xi, yi) }]} else {[]}
        | _ => []
        } ) |> List.flatten
      ) |> List.flatten;

    let setEnemy = (area, enemyInfo) => {
      let (x, y) = enemyInfo.location;
      Places.setEnemyAt(x, y, enemyInfo.enemy, 1., area) |> Rationale.Option.ofResult
    };

    let attackablePlaces = (targets, area) => targets 
        |> List.filter(pos => {
          let (x, y) = pos;
          Places.getPlace(x, y, area)
            |> Option.fmap(p => isPlayer(p))
            |> Option.default(false)
        }
      );

    let findTargets = (~range=1, enemyInfo) => {
      let (x, y) = enemyInfo.location;
      let targetX = [(x-range), x, (x+range)] |> List.filter(x => x >= 0);
      let targetY = [(y-range), y, (y+range)] |> List.filter(y => y >= 0);
      targetY |> List.map(y => (targetX |> List.map(x => (x, y)))) |> List.flatten;
    };

    let canAttack = (~range=1, area, enemyInfo) => {
      let targets = findTargets(~range=range, enemyInfo);
      let attackable = attackablePlaces(targets, area);
  
      if (List.length(attackable) >= 1) {
        true
      } else {
        false
      };
    };

    let attack = (enemyInfo, area) => {
      open Rationale.Option;
      let targets = enemyInfo |> findTargets |> attackablePlaces(_, area);
      if (List.length(targets) >= 1) {
        let (x,y) = List.hd(targets);
        let place = Places.getPlace(x, y, area);

        place >>= (place =>
            switch place.state {
              | Player(p) => Some(p)
              | _ => None;
            })
          |> Option.fmap((player:player) => { ...player, stats: {...player.stats, health: player.stats.health - 1} } )
          >>= (player => Places.setPlayerAt(x, y, player, 0., area) 
            |> Option.ofResult 
            |> Option.fmap(area => (area, player)));
        } else {
        None
      };
    };
  
    let enemyLogic = (activeEnemy, level, game) => {
      if (canAttack(level.map, activeEnemy)) {
        Js.Console.log(activeEnemy.enemy.name ++ " can attack");
  
        setEnemy(level.map, activeEnemy) 
          |> Option.bind(_, map => attack(activeEnemy, map))
          |> Option.fmap(r => {
          let (area, player) = r;
  
          {...level, map: area}
            |> lvl => World.updateLevel(lvl, game.world)
            |> w => {...game, world: w, player: player}
        })
      } else {
        Js.Console.log("is sleeping");
        /* Wait / sleep */
        setEnemy(level.map, activeEnemy) 
        |> Option.fmap(map => {...level, map: map })
        |> Option.fmap(l => World.updateLevel(l, game.world))
        |> Option.fmap(w => {...game, world: w})
      }
    };
  };