// player moves to mouse cursor.
// click to dash.

package {
	import flash.display.Sprite;
	[SWF(width="465", height="465", backgroundColor="#808080", frameRate="20")]
	public class RectDash extends Sprite {
		public function RectDash() {
			g_main = this; setup();
		}
	}
}

// import
import flash.display.*;
import flash.geom.*;
import flash.text.*;
import flash.events.*;
import flash.utils.*;
// import net.hires.debug.Stats;

// constant
const DISP_L:int = 465;
const FIELD_L:int = 300;
const FPS:uint = 20;
const DT:Number = 1.0 / FPS ;

const P_ST_MUTEKI:uint	= 1;
const P_ST_NORMAL:uint	= 2;
const P_ST_DASH:uint		= 3;
const P_ST_FUTTOBI:uint	= 4;
const P_ST_FADEOUT:uint	= 5;
const P_ST_DEAD:uint		= 6;
const E_ST_APPEAR:uint	= 11;
const E_ST_MUTEKI:uint	= 12;
const E_ST_NORMAL:uint	= 13;
const E_ST_FUTTOBI:uint	= 14;
const E_ST_FADEOUT:uint	= 15;
const E_ST_DEAD:uint		= 16;

const P_COL_NORMAL:uint = 0xffffff;
const P_COL_DASH:uint = 0xff3040;
const P_COL_FUTTOBI:uint = 0xa0a0a0;

const E_COL_NORMAL:uint = 0xa030a0;
const E_COL_FUTTOBI:uint = 0x802080;

// global variables
var g_main:Sprite;
var g_input:Input;
var g_disp:Disp;
var g_player:Player;
var g_enemies:Vector.<Enemy>;
var g_lifeText:TextField;
var g_scoreText:TextField;
var g_hiscoreText:TextField;
var g_noticeText:TextField;
var g_score:uint;
var g_hiscore:uint;

// 2D vector class
class Vec2 {
	public var x:Number = 0, y:Number = 0;
	public function Vec2(ax:Number = 0, ay:Number = 0) {
		x = ax; y = ay;
	}
	internal function set(ax:Number = 0, ay:Number = 0) : void {
		x = ax; y = ay;
	}

	internal function adds(v:Vec2) : Vec2 {
		x += v.x; y += v.y;
		return this;
	}
	internal function subs(v:Vec2) : Vec2 {
		x -= v.x; y -= v.y;
		return this;
	}
	internal function muls(n:Number) : Vec2 {
		x *= n; y *= n;
		return this;
	}
	internal function divs(n:Number) : Vec2 {
		x /= n; y /= n;
		return this;
	}

	internal function flipY() : Vec2 {
		y = -y;
		return this;
	}

	internal function len() : Number {
		return Math.sqrt(x * x + y * y);
	}
	internal function angle() : Number {
		return Math.atan2(y, x);
	}
}

// vector operator
internal function add(v1:Vec2, v2:Vec2) : Vec2 {
	return new Vec2(v1.x + v2.x, v1.y + v2.y);
}
internal function sub(v1:Vec2 , v2:Vec2) : Vec2 {
	return new Vec2(v1.x - v2.x, v1.y - v2.y);
}
internal function mul(v1:Vec2, n:Number) : Vec2 {
	return new Vec2(v1.x * n, v1.y * n);
}
internal function div(v1:Vec2 , n:Number ) : Vec2 {
	return new Vec2(v1.x / n, v1.y / n);
}
// create new Vec2 in Polar Coordinate System
internal function PCS(radius:Number, angle:Number) : Vec2 {
	return new Vec2(Math.cos(angle), Math.sin(angle)).muls(radius);
}

// normalize radian
internal function normalizeRad(a:Number) : Number {
	return a - Math.round(a / (2.0 * Math.PI)) * (2.0 * Math.PI);
}

// radian sub operation
internal function subRad(a:Number, b:Number) : Number {
	return normalizeRad(a - b);
}

// clamp x to range min~max
internal function clamp(x:Number, min:Number, max:Number) : Number {
	return Math.min(Math.max(x, min), max);
}
// get random value in range min~max
internal function randRange(min:Number, max:Number) : Number {
	return min + (max - min) * Math.random();
}

class Disp extends Sprite {
	public var m_center:Vec2;
	public var m_scale:Number;
	public var m_bitmapData:BitmapData;
	private var m_bitmap:Bitmap;

	public function Disp() : void {
		m_bitmapData = new BitmapData(DISP_L, DISP_L, false, 0);
		m_bitmap = new Bitmap(m_bitmapData);
		m_center = new Vec2(0, 0);
		m_scale = 1.0;

		addChild(m_bitmap);
	}

	internal function clear(color:uint) : void {
    m_bitmapData.fillRect(m_bitmapData.rect, color);
	}

	internal function beginDraw() : void {
		m_bitmapData.lock();
	}

	internal function endDraw() : void {
		m_bitmapData.unlock();
	}

	internal function drawRect(pos:Vec2, len:Number, color:uint) : void {
		var r:Rectangle = toRealRectangle(pos, len);
		m_bitmapData.fillRect(r, color);
	}

	internal function drawLine(pos:Vec2, len:Number, angle:Number, color:uint) : void {
		var ra:Vec2 = toRealCoord(pos);
		var rb:Vec2 = toRealCoord(add(pos, PCS(len, angle)));
		var d:Number = sub(ra, rb).len();
		var iter:uint = Math.abs(d) / 8;
		for (var i:uint = 0; i < iter; i++) {
			var m:Vec2 = add(mul(ra, i / iter), mul(rb, (iter - i) / iter));
			m_bitmapData.fillRect(new Rectangle(m.x - 2, m.y - 2, 4, 4), color);
		}
	}

	internal function drawCircle(pos:Vec2, len:Number, color:uint) : void {
		var arc:Number = 2.0 * Math.PI * len * m_scale;
		var iter:uint = arc / 3;
		var rpos:Vec2 = toRealCoord(pos);
		for (var i:uint = 0; i < iter; i++) {
			var m:Vec2 = add(rpos, PCS(len * m_scale, (i / iter) * 2.0 * Math.PI));
			m_bitmapData.fillRect(new Rectangle(m.x - 2, m.y - 2, 4, 4), color);
		}
	}

	// get real coord from virtual coord
	public function toRealCoord(pos:Vec2) : Vec2 {
		return sub(pos, m_center).muls(m_scale).flipY().adds(new Vec2(DISP_L / 2, DISP_L / 2));
	}

	// get virtual coord from real coord
	public function toGameCoord(pos:Vec2) : Vec2 {
		return sub(pos, new Vec2(DISP_L / 2, DISP_L / 2)).flipY().divs(m_scale).adds(m_center);
	}

	// get real rectangle from virtual coord and length
	private function toRealRectangle(pos:Vec2, len:Number) : Rectangle {
		var ul:Vec2 = toRealCoord(add(pos, new Vec2(-len / 2, len / 2)));
		return new Rectangle(ul.x, ul.y, len * m_scale, len * m_scale);
	}
}

// initialize something
internal function setup() : void {
	g_input = new Input();
	g_disp = new Disp();
	g_lifeText = new TextField();
	g_scoreText = new TextField();
	g_hiscoreText = new TextField();
	g_noticeText = new TextField();
	g_player = new Player();
	g_enemies = new Vector.<Enemy>();

	g_score = 0;
	g_hiscore = 0;

	g_scoreText.y += 20.0;
	g_hiscoreText.y += 40.0;
	g_noticeText.x += DISP_L / 2;
	g_noticeText.y += DISP_L / 2;
	g_noticeText.width = 350.0;
	g_noticeText.selectable = false;

	var proc:GameProcess = new GameProcess();
	proc.onInit();
	var t:Timer = new Timer(DT * 1000);

	// add event listener
	g_main.stage.addEventListener(MouseEvent.MOUSE_DOWN,
			function(e:MouseEvent) : void { g_input.onMouseDown(); } );
	g_main.stage.addEventListener(MouseEvent.MOUSE_UP,
			function(e:MouseEvent) : void { g_input.onMouseUp(); } );
	// t.addEventListener(TimerEvent.TIMER,
	// function(e:TimerEvent) : void { proc.onUpdate(); } );
	g_main.addEventListener(Event.ENTER_FRAME,
		function(e:Event) : void { proc.onUpdate(); } );

	// add stage children
	g_main.addChild(g_disp);
	// g_main.addChild(new Stats());
	g_main.addChild(g_lifeText);
	g_main.addChild(g_scoreText);
	g_main.addChild(g_hiscoreText);
	g_main.addChild(g_noticeText);

	t.start();
}

class State {
	internal function onInit() : void {
		// please override
	}
	internal function onExit() : void {
		// please override
	}
	internal function onUpdate() : void {
		// please override
	}
	internal function nextState() : Class {
		// please override
		return null;
	}
}

internal function drawField() : void {
	g_disp.beginDraw();
		g_disp.clear(0xff808080);
		g_disp.drawCircle(new Vec2(), FIELD_L + 10, 0xff000000);
		g_disp.drawCircle(new Vec2(), FIELD_L, 0xffa02020);
		for (var i:uint = 0; i < g_enemies.length; i++) {
			g_enemies[i].onRender();
		}
		g_player.onRender();
	g_disp.endDraw();
}

class TitleState extends State {

	internal override function onInit() : void {
		g_noticeText.text = "RectDash - 何か体当たりをするだけのゲーム";
	}

	internal override function onUpdate() : void {
		g_disp.m_center.adds(sub(new Vec2(), g_disp.m_center).muls(0.1));
		drawField();
	}

	internal override function nextState() : Class {
		if (g_input.btnDown()) {
			return GameState;
		} else {
			return null;
		}
	}
}

class GameState extends State {
	private var m_tick:uint;

	internal override function onInit() : void {
		g_score = 0;
		g_player = new Player();
		g_enemies = new Vector.<Enemy>();
		m_tick = 0;
		g_noticeText.text = "";
	}

	internal function newEnemy() : void {
		var angle:Number = m_tick * 7 / 3.0;
		g_enemies.push(
				new Enemy(
						PCS(FIELD_L * (Math.random() * 0.5 + 0.5), angle),
						angle,
						m_tick / (FPS * 40) + 1));
	}

	internal override function onUpdate() : void {
		m_tick++;
		g_player.onUpdate();

		if (m_tick % 10 % 2 == 0 && Math.random() * 30 < 3.0) {
			newEnemy();
		}

		if (g_enemies.length == 0) {
			newEnemy(); newEnemy(); newEnemy();
		}

		var i:int, j:int;

		for (i = 0; i < g_enemies.length; i++) {
			g_enemies[i].onUpdate();
		}

		for (i = 0; i < g_enemies.length; i++) {
			if (isBoxCollide(g_player, g_enemies[i])) {
				g_player.onCollide(g_enemies[i]);
				g_enemies[i].onCollide(g_player);
			}
		}
		g_player.clampPosition();

		for (i = 0; i < g_enemies.length - 1; i++) {
			for (j = i + 1; j < g_enemies.length; j++) {
				if (isBoxCollide(g_enemies[i], g_enemies[j])) {
					g_enemies[i].onCollide(g_enemies[j]);
					g_enemies[j].onCollide(g_enemies[i]);
				}
			}
			g_enemies[i].clampPosition();
		}

		for (i = g_enemies.length - 1; i >= 0; i--) {
			if (g_enemies[i].stat() == E_ST_DEAD) {
				g_enemies.splice(i, 1);
			}
		}
		drawField();

		g_scoreText.text = "score: " + g_score;
		if (g_score > g_hiscore) {
			g_hiscore = g_score;
			g_hiscoreText.text = "hiscore: " + g_hiscore;
		}
		g_lifeText.text = "life: ";
		for (i = 0; i < g_player.life(); i++) g_lifeText.appendText("o");
	}

	internal override function nextState() : Class {
		if (g_player.stat() == P_ST_DEAD && g_input.btnDown()) {
			return TitleState;
		} else {
			return null;
		}
	}
}

class GameProcess {
	protected var m_stat:State;

	// initialize
	internal function onInit() : void {
		m_stat = null;
		trans(TitleState);
	}

	// update
	internal function onUpdate() : void {
		var t:Vec2 = g_disp.toGameCoord(new Vec2(g_disp.mouseX, g_disp.mouseY));
		g_input.onUpdate(g_disp.toGameCoord(new Vec2(g_disp.mouseX, g_disp.mouseY)));
		if (m_stat) {
			m_stat.onUpdate();
			var cls:Class = m_stat.nextState();
			if (cls) {
				trans(cls);
			}
		}
	}

	private function trans(state : Class) : void {
		if (m_stat) m_stat.onExit();
		if (state != null) {
			m_stat = new state();
		} else {
			m_stat = null;
		}
		if (m_stat) m_stat.onInit();
	}
}

class Input {
	private var mb:Boolean;
	private var nmb:Boolean;
	private var pmb:Boolean;
	private var mv:Vec2;
	public function Input() {
		mb = pmb = nmb = false;
	}
	internal function onUpdate(v:Vec2) : void {
		pmb = nmb;
		nmb = mb;
		mv = v;
	}
	internal function pos() : Vec2 {
		return mv;
	}
	internal function btnDown() : Boolean {
		return nmb && (!pmb);
	}
	internal function btnHold() : Boolean {
		return nmb;
	}
	internal function onMouseDown() : void {
		mb = true;
	}
	internal function onMouseUp() : void {
		mb = false;
	}
}

class Box {
	protected var m_pos:Vec2;
	protected var m_velo:Number;
	protected var m_angle:Number;
	protected var m_color:uint;
	protected var m_len:Number;
	protected var m_force:Vec2;
	protected var m_stat:uint;
	protected var m_life:uint;
	public var m_collEnable:Boolean;

	// constructor
	public function Box(pos:Vec2, velo:Number, angle:Number, color:uint, len:Number) {
		m_pos = pos; m_velo = velo; m_angle = angle;
		m_color = color; m_len = len;
		m_force = new Vec2();
	}

	// apply velocity to position
	internal function onUpdate() : void {
		m_pos.adds(PCS(m_velo * DT, m_angle));
		m_pos.adds(m_force);
		m_force.set(0.0, 0.0);
	}

	internal function onCollide(e:Box) : void {
		var d:Vec2 = sub(e.m_pos, m_pos);
		var l:Number = (m_len + e.m_len) / 2;
		var dl:Number = d.len();
		m_force.subs(d.muls((l - dl) * 0.5 / dl))
	}

	internal function clampPosition() : void {
		var len:Number = m_pos.len();
		var lim:Number = FIELD_L - m_len;
		if (len > lim) {
			m_pos.muls(lim / len);

			if (m_stat == E_ST_FUTTOBI) {
				if (m_life > 0) {
					m_velo = 0;
					m_stat = E_ST_MUTEKI;
					m_color = E_COL_NORMAL;
					m_collEnable = false;
				} else {
					m_stat = E_ST_FADEOUT;
					// m_color = e_col_normal;
					m_collEnable = false;
				}
			}
		}
		// var l:Number = FIELD_L / 2 - m_len / 2;
		// m_pos.x = clamp(m_pos.x, -l, l);
		// m_pos.y = clamp(m_pos.y, -l, l);
	}

		/*
		if (Math.abs(d.x) > Math.abs(d.y)) {
			if (d.x < 0) {
				m_force.x += (l + d.x) / 2;
			} else {
				m_force.x -= (l - d.x) / 2;
			}
		} else {
			if (d.y < 0) {
				m_force.y += (l + d.y) / 2;
			} else {
				m_force.y -= (l - d.y) / 2;
			}
		}
		*/

	// draw box
	internal function onRender() : void {
		g_disp.drawRect(m_pos, m_len + 2, 0x000000);
		g_disp.drawRect(m_pos, m_len, m_color);
	}

	internal function pos() : Vec2 { return m_pos; }
	internal function len() : Number { return m_len; }
	internal function stat() : uint { return m_stat; }
	internal function life() : uint { return m_life; }

}

// check box collision
internal function isBoxCollide(b1:Box, b2:Box) : Boolean {
	if (!b1.m_collEnable || !b2.m_collEnable) return false;
	var dv:Vec2 = sub(b1.pos(), b2.pos());
	var len:Number = (b1.len() + b2.len()) / 2;
	// return (Math.abs(dv.x) < len) && (Math.abs(dv.y) < len);
	return dv.len() < len;
}

class Player extends Box {
	private var m_tick:uint;
	private var m_dashLen:Number;

	private static const MUTEKI_F:uint = 60;
	private static const DASH_F:uint = 12;
	private static const DASH_V:Number = 250.0;
	private static const FUTTOBI_F:uint = 25;
	private static const FUTTOBI_V:Number = 200.0;
	private static const FADEOUT_F:uint = 50;
	private static const LEN:Number = 20.0
	private static const DASH_LEN:Number = 15.0
	private static const vr:Number = 3.0;
	private static const vl:Number = 70.0;

	// constructor
	public function Player() {
		super(new Vec2(0, 0), 0.0, 0.0, P_COL_NORMAL, LEN);
		m_stat = P_ST_MUTEKI;
		m_tick = 0;
		m_life = 5;
		m_collEnable = false;
	}

	internal override function onUpdate() : void {
		// distance from mouse cursor to player
		var dv:Vec2 = sub(g_input.pos(), m_pos);
		m_tick++;

		var prev_stat:uint = m_stat;
		switch(m_stat) {
			case P_ST_MUTEKI:
				if (m_tick > MUTEKI_F) {
					m_stat = P_ST_NORMAL;
					m_collEnable = true;
				}

			// 通常状態
			case P_ST_NORMAL:
				m_len += (LEN - m_len) * 0.01;

				m_angle = dv.angle();
				m_velo = Math.min(dv.len() * vr, vl);

				if (g_input.btnDown()) {
					m_stat = P_ST_DASH;
					m_velo = DASH_V;
					m_color = P_COL_DASH;
					m_collEnable = true;
					m_dashLen = m_len + DASH_LEN;
				}
				break;

			case P_ST_DASH:

				m_len += (m_dashLen - m_len) * 0.1;
				if (m_tick > DASH_F) {
					m_stat = P_ST_NORMAL;
					m_color = P_COL_NORMAL;
					m_collEnable = true;
				}
				break;

			case P_ST_FUTTOBI:
				m_velo = FUTTOBI_V;

				if (m_tick > FUTTOBI_F) {
					if (m_life > 0) {
						m_stat = P_ST_MUTEKI;
						m_color = P_COL_NORMAL;
						m_collEnable = false;
					} else {
						m_stat = P_ST_FADEOUT;
						// m_color = P_COL_NORMAL;
						m_collEnable = false;
					}
				}
				break;

			case P_ST_FADEOUT:
			case P_ST_DEAD:
				m_len += -m_len * 0.03;
				if (m_tick > FADEOUT_F) {
					m_stat = P_ST_DEAD;
					g_noticeText.text = "GAME OVER";
				}
				break;
		}

		if (prev_stat != m_stat) {
			m_tick = 0;
		}

		super.onUpdate();

		// set camera to player
		g_disp.m_center.adds(sub(m_pos, g_disp.m_center).muls(0.1));
	}

	internal override function onRender() : void {
		if (m_stat == P_ST_MUTEKI && (m_tick / 2) % 2 == 0) {
		} else {
			super.onRender();
		}
		g_disp.drawLine(m_pos, m_velo, m_angle, 0xa08080);
	}

	internal override function onCollide(e:Box) : void {
		if (e.stat() == E_ST_NORMAL && m_stat == P_ST_NORMAL) {
			m_life--;
			m_stat = P_ST_FUTTOBI;
			m_color = P_COL_FUTTOBI;
			m_angle = sub(m_pos, e.pos()).angle();
			m_tick = 0;
			m_collEnable = true;
		} else {
			super.onCollide(e);
		}
	}

}

class Enemy extends Box {
	private var m_tick:uint;

	private static const LEN:Number = 20.0;
	private static const APPEAR_F:uint = 20;
	private static const MUTEKI_F:uint = 40;
	private static const NORMAL_V:Number = 75.0;
	private static const FUTTOBI_V:Number = 200.0;
	private static const FUTTOBI_F:uint = 20;
	private static const FADEOUT_F:uint = 30;

	public function Enemy(pos:Vec2, angle:Number, life:uint) {
		super(pos, 0.0, angle, E_COL_NORMAL, LEN);
		m_stat = E_ST_APPEAR;
		m_tick = 0;
		m_life = life;
		m_collEnable = false;
	}

	internal override function onUpdate() : void {
		m_tick++;

		var prev_stat:uint = m_stat;
		switch (m_stat) {
			case E_ST_APPEAR:
				if (m_tick > APPEAR_F) {
					m_stat = E_ST_NORMAL;
					m_collEnable = true;
				}
				break;

			case E_ST_MUTEKI:
				if (m_tick > MUTEKI_F) {
					m_stat = E_ST_NORMAL;
					m_collEnable = true;
				}

			case E_ST_NORMAL:
				var dv:Vec2 = sub(g_player.pos(), pos());
				m_angle += subRad(dv.angle(), m_angle) * 0.08;
				if (m_stat == E_ST_NORMAL) {
					m_velo += (NORMAL_V - m_velo) * 0.03;
				}
				break;

			case E_ST_FUTTOBI:
				m_velo = FUTTOBI_V;
				if (m_tick > FUTTOBI_F) {
					/*
					if (m_life > 0) {
						m_velo = 0;
						m_stat = e_st_muteki;
						m_color = e_col_normal;
						m_collenable = false;
					} else {
						m_stat = e_st_fadeout;
						// m_color = e_col_normal;
						m_collenable = false;
					}
					*/
				}
				break;

			case E_ST_FADEOUT:
				m_len += -m_len * 0.03;
				if (m_tick > FADEOUT_F) {
					m_stat = E_ST_DEAD;
				}
				break;
		}

		if (m_stat != prev_stat) {
			m_tick = 0;
		}

		super.onUpdate(); }

	internal override function onRender() : void {
		if ((m_stat == E_ST_MUTEKI || m_stat == E_ST_APPEAR)
				&& (m_tick % 2) == 0) {
		} else {
			super.onRender();
		}
	}

	internal override function onCollide(e:Box) : void {
		if (m_stat == E_ST_NORMAL) {
			if (e.stat() == P_ST_DASH || e.stat() == E_ST_FUTTOBI) {
				m_life--;
				if (m_life == 0) g_score++;
				m_stat = E_ST_FUTTOBI;
				m_color = E_COL_FUTTOBI;
				m_tick = 0;
				m_angle = sub(m_pos, e.pos()).angle();
				m_collEnable = true;
			} else {
				super.onCollide(e);
			}
		}
	}
}
