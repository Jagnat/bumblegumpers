using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using System;

namespace questionmark
{
	public struct InputState
	{
		public bool leftPressed, rightPressed, upPressed, downPressed, attackPressed, usePressed, shieldPressed;
	}

	public class Game1 : Game
	{
		Player player;
		World world;

		Render renderer;

		KeyboardState pastKeyState, currentKeyState;

		GraphicsDeviceManager graphics;

		InputState input;

		public Game1()
		{
			IsMouseVisible = true;
			Window.Title = "?????";
			IsFixedTimeStep = true;
			TargetElapsedTime = new TimeSpan(166667);
			
			graphics = new GraphicsDeviceManager(this);
			graphics.PreferredBackBufferWidth = 768;
			graphics.PreferredBackBufferHeight = 576;
			Content.RootDirectory = "Content";
		}

		protected override void Initialize()
		{
			renderer = new Render(this);

			world = new World(16, 12);
			player = new Player(1, 10);
			pastKeyState = Keyboard.GetState();
			base.Initialize();
		}

		protected override void LoadContent()
		{
			renderer.loadGraphics(Content);
		}

		protected override void UnloadContent()
		{
			
		}

		protected override void Update(GameTime gameTime)
		{
			if (GamePad.GetState(PlayerIndex.One).Buttons.Back == ButtonState.Pressed || Keyboard.GetState().IsKeyDown(Keys.Escape))
				Exit();
			updateInput();

			player.update(input, world);

			base.Update(gameTime);
		}

		void updateKey(Keys key, out bool val)
		{
			if (currentKeyState.IsKeyDown(key) && pastKeyState.IsKeyUp(key))
				val = true;
			else
				val = false;
		}

		void updateInput()
		{
			currentKeyState = Keyboard.GetState();
			updateKey(Keys.Left, out input.leftPressed);
			updateKey(Keys.Right, out input.rightPressed);
			updateKey(Keys.Up, out input.upPressed);
			updateKey(Keys.Down, out input.downPressed);
			updateKey(Keys.Z, out input.attackPressed);
			updateKey(Keys.X, out input.usePressed);
			updateKey(Keys.C, out input.shieldPressed);

			pastKeyState = currentKeyState;
		}

		protected override void Draw(GameTime gameTime)
		{
			GraphicsDevice.Clear(Color.CornflowerBlue);

			renderer.renderWorld(world);
			renderer.renderPlayer(player);

			base.Draw(gameTime);
		}
	}
}
