using Microsoft.Xna.Framework;
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

		public struct PanelState
		{
			public bool open;
			public int height;
			public string title;

			public PanelState(bool open, int height, string title)
			{
				this.open = open;
				this.height = height;
				this.title = title;
			}
		}

		ModeSelect modeSelectState = ModeSelect.MAIN;
		PanelState blockPanelState = new PanelState(true, 60, "Blocks");

		int offs = 0;

		public Editor()
		{

		}

		public void updateEditor(InputState input)
		{
			
		}

		public void resize(Rectangle bounds)
		{
			this.bounds = bounds;
		}

		public void renderEditor(Render renderer)
		{
			renderer.startDebugRender();

			// background rectangle
			renderer.renderDebugRect(new Rectangle(0, 0, panelWidth, bounds.Height), new Color(Color.DarkSlateGray, 0.8f), 1.0f);

			// scrollbar
			renderer.renderDebugRect(new Rectangle(panelWidth, 0, scrollWidth, bounds.Height), new Color(Color.Gray, .8f), 1.0f);

			offs = 0;
			renderModeSelect(renderer);

			offs += 10;
			renderPanel1(renderer);

			offs += 10;

			renderer.endDebugRender();
		}

		public void renderPanel1(Render renderer)
		{
			renderer.renderDebugRect(new Rectangle(0, offs, panelWidth, 24), Color.Violet, 0.8f);
			renderer.renderDebugText(panelWidth / 2, offs + 12, Color.White, 0.7f, blockPanelState.title, true, true);
			offs += 24;

			if (blockPanelState.open)
			{
				renderer.renderDebugRect(new Rectangle(0, offs, panelWidth, blockPanelState.height), new Color(Color.DarkSlateBlue, 0.8f), 0.8f);

				renderer.renderDebugRect(new Rectangle(12 + 0 * 48, offs + 12, 36, 36), Color.HotPink, 0.7f);
				renderer.renderDebugText(12 + 18, offs + 12 + 18, Color.Black, 0.6f, "E", true, true);
				renderer.renderDebugTile(new Rectangle(12 + 1 * 48, offs + 12, 36, 36), TileType.DIRT, 0, 0.7f);
				renderer.renderDebugTile(new Rectangle(12 + 2 * 48, offs + 12, 36, 36), TileType.BRICK, 0, 0.7f);
				renderer.renderDebugTile(new Rectangle(12 + 3 * 48, offs + 12, 36, 36), TileType.WOOD, 0, 0.7f);
				renderer.renderDebugTile(new Rectangle(12 + 4 * 48, offs + 12, 36, 36), TileType.SQUARE, 0, 0.7f);
				renderer.renderDebugTile(new Rectangle(12 + 5 * 48, offs + 12, 36, 36), TileType.GRASS, 0, 0.7f);
				renderer.renderDebugTile(new Rectangle(12 + 6 * 48, offs + 12, 36, 36), TileType.LADDER, 0, 0.7f);
			}
		}

		public void renderModeSelect(Render renderer)
		{
			Color s = Color.Red;
			Color n = new Color(Color.White, 0.7f);
			offs += 10;
			renderer.renderDebugRect(new Rectangle(1 * 72 - 10, offs, 20, 20), modeSelectState == ModeSelect.BACKGROUND ? s : n, 0.8f);
			renderer.renderDebugRect(new Rectangle(2 * 72 - 10, offs, 20, 20), modeSelectState == ModeSelect.MAIN ? s : n, 0.8f);
			renderer.renderDebugRect(new Rectangle(3 * 72 - 10, offs, 20, 20), modeSelectState == ModeSelect.FOREGROUND ? s : n, 0.8f);
			renderer.renderDebugRect(new Rectangle(4 * 72 - 10, offs, 20, 20), modeSelectState == ModeSelect.COLLISION ? s : n, 0.8f);
			offs += 20;

			renderer.renderDebugText(1 * 72, offs, Color.White, 0.0f, "B", true);
			renderer.renderDebugText(2 * 72, offs, Color.White, 0.0f, "M", true);
			renderer.renderDebugText(3 * 72, offs, Color.White, 0.0f, "F", true);
			renderer.renderDebugText(4 * 72, offs, Color.White, 0.0f, "C", true);

			offs += 20;
		}
	}
}
