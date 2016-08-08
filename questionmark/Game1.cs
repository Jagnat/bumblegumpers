using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using System;

namespace questionmark
{
	public class Game1 : Game
	{
		World world;

		Render renderer;

		GraphicsDeviceManager graphics;

		public Game1()
		{
			IsMouseVisible = true;
			Window.Title = "?????";
			IsFixedTimeStep = true;
			TargetElapsedTime = new TimeSpan(166667);
			
			graphics = new GraphicsDeviceManager(this);
			graphics.PreferredBackBufferWidth = 640;
			graphics.PreferredBackBufferHeight = 480;
			Content.RootDirectory = "Content";
		}

		protected override void Initialize()
		{
			renderer = new Render(this);

			world = new World(16, 16);
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

			base.Update(gameTime);
		}

		protected override void Draw(GameTime gameTime)
		{
			GraphicsDevice.Clear(Color.Black);

			renderer.renderWorld(world);

			base.Draw(gameTime);
		}
	}
}
