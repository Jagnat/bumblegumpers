using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using System;

namespace questionmark
{
	public struct InputState
	{
		public bool leftPressed, rightPressed, jumpPressed, attackPressed, usePressed, shieldPressed;
	}

	public class Game1 : Game
	{
		Player player;
		World world;

		Render renderer;

		KeyboardState pastKeyState;

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


			base.Update(gameTime);
		}

		void updateInput()
		{
			KeyboardState currentKeyState = Keyboard.GetState();
			if (currentKeyState.IsKeyDown(Keys.Left) && pastKeyState.IsKeyUp(Keys.Left))
				input.leftPressed = true;
			else
				input.leftPressed = false;

			if (currentKeyState.IsKeyDown(Keys.Right) && pastKeyState.IsKeyUp(Keys.Right))
				input.rightPressed = true;
			else
				input.rightPressed = false;

			if (currentKeyState.IsKeyDown(Keys.Up) && pastKeyState.IsKeyUp(Keys.Up))
				input.jumpPressed = true;
			else
				input.jumpPressed = false;

			if (currentKeyState.IsKeyDown(Keys.Z) && pastKeyState.IsKeyUp(Keys.Z))
				input.attackPressed = true;
			else
				input.attackPressed = false;

			if (currentKeyState.IsKeyDown(Keys.X) && pastKeyState.IsKeyUp(Keys.X))
				input.usePressed = true;
			else
				input.usePressed = false;

			if (currentKeyState.IsKeyDown(Keys.C) && pastKeyState.IsKeyUp(Keys.C))
				input.shieldPressed = true;
			else
				input.shieldPressed = false;
			pastKeyState = currentKeyState;
		}

		protected override void Draw(GameTime gameTime)
		{
			GraphicsDevice.Clear(Color.CornflowerBlue);

			renderer.renderWorld(world);

			base.Draw(gameTime);
		}
	}
}
