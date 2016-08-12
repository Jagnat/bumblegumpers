using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using System;

namespace bumblegumpers
{
	public struct InputState
	{
		public bool leftPressed, rightPressed, upPressed, downPressed, attackPressed, usePressed, shieldPressed;
		public KeyboardState currentKeyboard, pastKeyboard;
		public MouseState currentMouse, pastMouse;
	}

	public class Game1 : Game
	{
		Player player;
		World world;

		Render renderer;

		GraphicsDeviceManager graphics;

		InputState input;

		Editor editor;

		bool inEditor = false;

		public Game1()
		{
			IsMouseVisible = true;
			Window.Title = "?????";
			IsFixedTimeStep = true;
			TargetElapsedTime = new TimeSpan(166667);
			
			graphics = new GraphicsDeviceManager(this);
			graphics.PreferredBackBufferWidth = 768;
			graphics.PreferredBackBufferHeight = 576;
			graphics.ApplyChanges();
			Content.RootDirectory = "Content";
		}

		protected override void Initialize()
		{
			renderer = new Render(this);

			editor = new Editor();

			world = new World(16, 12);
			player = new Player(1, 10);
			input.pastKeyboard = Keyboard.GetState();
			input.pastMouse = Mouse.GetState();
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

			bool toggleEdit;
			updateKey(Keys.E, out toggleEdit);
			if (toggleEdit)
			{
				inEditor = !inEditor;
				if (inEditor)
				{
					graphics.PreferredBackBufferWidth = 1400;
					graphics.PreferredBackBufferHeight = 900;
					graphics.ApplyChanges();
					renderer.refreshCamera();
				}
				else
				{
					graphics.PreferredBackBufferWidth = 768;
					graphics.PreferredBackBufferHeight = 576;
					graphics.ApplyChanges();
					renderer.refreshCamera();
				}
			}

			if (!inEditor)
				player.update(input, world);

			input.pastMouse = input.currentMouse;
			input.pastKeyboard = input.currentKeyboard;
			base.Update(gameTime);
		}

		void updateKey(Keys key, out bool val)
		{
			if (input.currentKeyboard.IsKeyDown(key) && input.pastKeyboard.IsKeyUp(key))
				val = true;
			else
				val = false;
		}

		void updateInput()
		{
			input.currentMouse = Mouse.GetState();
			input.currentKeyboard = Keyboard.GetState();
			updateKey(Keys.Left, out input.leftPressed);
			updateKey(Keys.Right, out input.rightPressed);
			updateKey(Keys.Up, out input.upPressed);
			updateKey(Keys.Down, out input.downPressed);
			updateKey(Keys.Z, out input.attackPressed);
			updateKey(Keys.X, out input.usePressed);
			updateKey(Keys.C, out input.shieldPressed);
		}

		protected override void Draw(GameTime gameTime)
		{
			GraphicsDevice.Clear(Color.DarkSlateGray);
			renderer.startRender();
			renderer.renderWorld(world);
			renderer.renderPlayer(player);
			renderer.endRender();

			if (inEditor)
				editor.renderEditor(renderer);

			base.Draw(gameTime);
		}
	}
}
