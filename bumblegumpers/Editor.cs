using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Input;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace bumblegumpers
{
	public class Editor
	{
		Rectangle bounds;

		int panelWidth = 360;
		int scrollWidth = 24;

		// GUI state;
		enum ModeSelect
		{
			BACKGROUND,
			MAIN,
			FOREGROUND,
			COLLISION
		}

		ModeSelect modeSelect = ModeSelect.MAIN;
		BlockPanel blockPanel = new BlockPanel(true);

		int offs = 0;

		Render renderer;
		World world;

		public Editor(Render renderer)
		{
			this.renderer = renderer;
		}

		public void setWorld(World world)
		{
			this.world = world;
		}

		public void update(InputState input)
		{
			if (input.currentMouse.LeftButton == ButtonState.Released)
			{
				
			}
		}

		public void resize(Rectangle bounds)
		{
			this.bounds = bounds;
		}

		public void renderEditor()
		{
			renderer.startRender();
			renderer.renderWorld(world);
			renderer.endRender();

			renderer.startDebugRender();

			// background rectangle
			renderer.renderDebugRect(new Rectangle(0, 0, panelWidth, bounds.Height), new Color(Color.DarkSlateGray, 0.8f), 1.0f);

			// scrollbar
			renderer.renderDebugRect(new Rectangle(panelWidth, 0, scrollWidth, bounds.Height), new Color(Color.Gray, .8f), 1.0f);

			offs = 0;
			renderModeSelect();

			offs += 12;
			renderPanel1();

			offs += 12;
			renderPanel2();

			renderer.endDebugRender();
		}

		public void renderPanel1()
		{
			renderer.renderDebugRect(new Rectangle(0, offs, panelWidth, 24), Color.Violet, 0.8f);
			renderer.renderDebugText(panelWidth / 2, offs + 12, Color.Black, 0.7f, blockPanel.title, true, true);
			offs += 24;

			if (blockPanel.open)
			{
				renderer.renderDebugRect(new Rectangle(0, offs, panelWidth, blockPanel.height), new Color(Color.DarkSlateBlue, 0.8f), 0.8f);

				int s = 21;
				int p = s + 36;

				offs += s;

				if (blockPanel.showCollision)
				{

				}
				else
				{
					for (int c = 0; c < blockPanel.numTileRows; ++c)
					{
						for (int r = 0; r < 6; ++r)
						{
							if (c * 6 + r + 1 > blockPanel.numTileTypes)
								break;
							TileType t = (TileType)(c * 6 + r);
							if (t == TileType.EMPTY)
							{
								renderer.renderDebugRect(new Rectangle(s + r * p, offs, 36, 36), Color.HotPink, 0.7f);
								renderer.renderDebugText(s + 18, offs + 18, Color.Black, 0.6f, "E", true, true);
							}
							else
								renderer.renderDebugTile(new Rectangle(s + r * p, offs, 36, 36), t, 0, 0.7f);
						}
						offs += p;
					}
				}
			}
		}

		public void renderPanel2()
		{
			renderer.renderDebugRect(new Rectangle(0, offs, panelWidth, 24), Color.Violet, 0.8f);
			renderer.renderDebugText(panelWidth / 2, offs + 12, Color.Black, 0.7f, "Interactables", true, true);
			offs += 24;
		}

		public void renderModeSelect()
		{
			Color s = Color.Red;
			Color n = new Color(Color.White, 0.7f);
			offs += 10;
			renderer.renderDebugRect(new Rectangle(1 * 72 - 10, offs, 20, 20), modeSelect == ModeSelect.BACKGROUND ? s : n, 0.8f);
			renderer.renderDebugRect(new Rectangle(2 * 72 - 10, offs, 20, 20), modeSelect == ModeSelect.MAIN ? s : n, 0.8f);
			renderer.renderDebugRect(new Rectangle(3 * 72 - 10, offs, 20, 20), modeSelect == ModeSelect.FOREGROUND ? s : n, 0.8f);
			renderer.renderDebugRect(new Rectangle(4 * 72 - 10, offs, 20, 20), modeSelect == ModeSelect.COLLISION ? s : n, 0.8f);
			offs += 20;

			renderer.renderDebugText(1 * 72, offs, Color.White, 0.0f, "B", true);
			renderer.renderDebugText(2 * 72, offs, Color.White, 0.0f, "M", true);
			renderer.renderDebugText(3 * 72, offs, Color.White, 0.0f, "F", true);
			renderer.renderDebugText(4 * 72, offs, Color.White, 0.0f, "C", true);

			offs += 20;
		}
	}

	public class BlockPanel
	{
		public bool open;
		public string title;
		public int height;
		public int numTileTypes;
		public int numCollisionTypes;
		public int numTileRows;
		public int numCollisionRows;

		public bool showCollision = false;

		public BlockPanel(bool open)
		{
			this.open = open;
			numTileTypes = Enum.GetNames(typeof(TileType)).Length;
			numCollisionTypes = Enum.GetNames(typeof(TileCollision)).Length;

			numTileRows = (int)Math.Ceiling((float)numTileTypes / 6.0f);
			numCollisionRows = (int)Math.Ceiling((float)numCollisionTypes / 6.0f);
			setMode(false);
		}

		public void setMode(bool showCollision)
		{
			if (showCollision)
			{
				title = "Collision";
				height = numCollisionRows * 57 + 21;
			}
			else
			{
				title = "Blocks";
				// 36n + 21(n+1)
				height = numTileRows * 57 + 21;
			}
		}
	}
}
