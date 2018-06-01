
module Positions = Positions.BasicPositions;
module EnemyLoop = Enemy.CreateEnemyLoop(Positions, Level.Area, World.World);
module BasicTurnLoop = Gameloop.CreateGameLoop(Positions, EnemyLoop);
module Game = Bouken.CreateGame(BasicTurnLoop, World.World, World.Builder);