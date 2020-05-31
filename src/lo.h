
#ifndef LO_H_INCLUDED
#define LO_H_INCLUDED

#include <assert.h>
#include <list>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

typedef	unsigned	char u8;
typedef	unsigned	short u16;
typedef	unsigned	int u32;
typedef	signed		char i8;
typedef	signed		short i16;
typedef	signed		int i32;
typedef	float		f32;
typedef	double		f64;

#define LO_ASSERT(e) assert(e)
#define LO_TRACE(...) printf(__VA_ARGS__) ; printf("\n")

enum LO_TOKEN_TYPE_ENUM
{
	LO_TOKEN_TYPE_EOF,
	LO_TOKEN_TYPE_SPACE,
	LO_TOKEN_TYPE_LPAREN,
	LO_TOKEN_TYPE_RPAREN,
	LO_TOKEN_TYPE_ATOM,
	LO_TOKEN_TYPE_MAX,
};

const char* LO_TOKEN_NAME[LO_TOKEN_TYPE_MAX] = {
	"LO_TOKEN_TYPE_EOF",
	"LO_TOKEN_TYPE_SPACE",
	"LO_TOKEN_TYPE_LPAREN",
	"LO_TOKEN_TYPE_RPAREN",
	"LO_TOKEN_TYPE_ATOM",
};

struct LoToken
{

public:

	std::string buffer;
	u32 type;

	const char* getTypeName() const
	{
		LO_ASSERT(type < LO_TOKEN_TYPE_MAX);
		return LO_TOKEN_NAME[type];
	}

	LoToken(u32 type, const std::string& buffer)
		: type(type)
		, buffer(buffer)
	{
	}

}; // struct LoToken

enum LO_CELL_TYPE_ENUM
{
	LO_CELL_TYPE_NIL,
	LO_CELL_TYPE_INT,
	LO_CELL_TYPE_FLOAT,
	LO_CELL_TYPE_CONS,
	LO_CELL_TYPE_SYMBOL,
	LO_CELL_TYPE_ROUTINE,
	LO_CELL_TYPE_FORWARDING,
	LO_CELL_TYPE_MAX,
};

const char* LO_CELL_NAME[LO_CELL_TYPE_MAX] = {
	"LO_CELL_TYPE_NIL",
	"LO_CELL_TYPE_INT",
	"LO_CELL_TYPE_FLOAT",
	"LO_CELL_TYPE_CONS",
	"LO_CELL_TYPE_SYMBOL",
	"LO_CELL_TYPE_ROUTINE",
	"LO_CELL_TYPE_FORWARDING",
};

struct LoCell;
class Lo;
typedef LoCell* (*LoRoutine) (Lo*, LoCell*);

struct LoCons
{
	LoCell* car;
	LoCell* cdr;
};

struct LoCell
{
	u32 type;
	bool marked;
	union {
		LoCons cons_value;
		i32 int_value;
		f32 float_value;
		u32 symbol_value;
		LoRoutine routine_value;
		LoCell* forwarding_value;
	} value;

public:

	void setNil()
	{
		type = LO_CELL_TYPE_NIL;
	}

	void setCons(LoCell* _car, LoCell* _cdr)
	{
		type = LO_CELL_TYPE_CONS;
		value.cons_value.car = _car;
		value.cons_value.cdr = _cdr;
	}

	void setCons(const LoCons& cons)
	{
		type = LO_CELL_TYPE_CONS;
		value.cons_value = cons;
	}

	void setInt(i32 v)
	{
		type = LO_CELL_TYPE_INT;
		value.int_value = v;
	}

	void setFloat(f32 v)
	{
		type = LO_CELL_TYPE_FLOAT;
		value.float_value = v;
	}

	void setSymbol(u32 v)
	{
		type = LO_CELL_TYPE_SYMBOL;
		value.symbol_value = v;
	}

	void setRoutine(LoRoutine v)
	{
		type = LO_CELL_TYPE_ROUTINE;
		value.routine_value = v;
	}

	void setForwarding(LoCell* v)
	{
		type = LO_CELL_TYPE_FORWARDING;
		value.forwarding_value = v;
	}

}; // class LoCell

enum LO_OP_CODE_ENUM
{
	LO_OP_CODE_NOP,
	LO_OP_CODE_PUSH_INT,
	LO_OP_CODE_PUSH_FLOAT,
	LO_OP_CODE_PUSH_SYMBOL,
	LO_OP_CODE_PUSH_ROUTINE,
};

struct LoOp
{
	u8 code;
	u8 v1;
	u8 v2;
	u8 v3;
}; // struct LoOp

class Lo
{

	static const int CELL_HEAP_BYTES = 1024 * 2; // 2KB
	static const int CELL_HEAP_SIZE  = CELL_HEAP_BYTES / sizeof(LoCell);
	static const int CELL_HEAP_COUNT = 2;

	std::list<LoToken> m_tokens;
	std::vector<std::string> m_symbols;
	std::vector<LoCell*> m_env;
	std::vector<LoCell*> m_stack;
	LoCell* m_heapBuffer[CELL_HEAP_COUNT];
	LoCell* m_currentHeap;
	LoCell* m_currentHeapEnd;
	int m_currentHeapIndex;

private:

	static bool isNumber(char c)
	{
		return ('0' <= c && c <= '9');
	}

public:

	LoCell* newCell()
	{
		if (m_currentHeap == m_currentHeapEnd) {
			collectGarbage();
		}

		LoCell* ptr = m_currentHeap++;
		ptr->setNil();

		return ptr;
	}

	u32 intern(const char* name)
	{
		for (int i = 0; i < m_symbols.size(); i++) {
			if (m_symbols[i] == name) {
				return i;
			}
		}
		m_symbols.push_back(name);
		return m_symbols.size() - 1;
	}

	const char* symbolName(u32 index)
	{
		return m_symbols[index].c_str();
	}

	LoCell* readAtom(LoToken tok)
	{
		LoCell* cur = newCell();
		m_stack.push_back(cur);
		if ((tok.buffer.size() > 0 && isNumber(tok.buffer[0]))
				|| (tok.buffer.size() > 2 && tok.buffer[0] == '-' && isNumber(tok.buffer[1]))) {
			// TODO: float
			cur->setInt(atoi(tok.buffer.c_str()));
		} else {
			cur->setSymbol(intern(tok.buffer.c_str()));
		}
		m_stack.pop_back();
		return cur;
	}

	LoCell* readCell()
	{
		LoToken tok = pop();
		switch (tok.type) {
			case LO_TOKEN_TYPE_EOF:
			case LO_TOKEN_TYPE_SPACE:
				return NULL;

			case LO_TOKEN_TYPE_LPAREN:
				{
					LoCell* top = newCell();
					m_stack.push_back(top);
					LoCell* cur = top;
					while (m_tokens.front().type != LO_TOKEN_TYPE_RPAREN) {
						LoCell* next = newCell();
						cur->setCons(readCell(), next);
						cur = next;
					}
					pop();
					cur->setNil();
					m_stack.pop_back();
					return top;
				}

			case LO_TOKEN_TYPE_ATOM:
				return readAtom(tok);
		}
	}

	void print(LoCell* c)
	{
		switch(c->type) {
			case LO_CELL_TYPE_NIL:
				printf("nil");
				break;
			case LO_CELL_TYPE_INT:
				printf("%d", c->value.int_value);
				break;
			case LO_CELL_TYPE_FLOAT:
				printf("%f", c->value.float_value);
				break;
			case LO_CELL_TYPE_SYMBOL:
				printf("%s", symbolName(c->value.symbol_value));
				break;

			case LO_CELL_TYPE_CONS:
				printf("(");
				{
					for(;;) {
						print(c->value.cons_value.car);
						c = c->value.cons_value.cdr;
						if (c->type != LO_CELL_TYPE_NIL) {
							printf(" ");
						} else {
							break;
						}
					}
				}
				printf(")");
				break;
		}
	}

	// copy cell to buffer
	static void gc_copy(LoCell* c, LoCell** buffer)
	{
		// copy
		**buffer = *c;

		// set forwarding pointer
		c->setForwarding(*buffer);

		// increment buffer
		(*buffer)++;
	}

	// copy cell and write next address
	static void gc_write(LoCell** c, LoCell** buffer)
	{
		LoCell* cur = *c;
		if (cur->type != LO_CELL_TYPE_FORWARDING) {
			gc_copy(cur, buffer);
		}
		LO_ASSERT(cur->type == LO_CELL_TYPE_FORWARDING);
		*c = cur->value.forwarding_value;
	}

	void collectGarbage()
	{
		LO_TRACE("collectGarbage()");
		int nextHeapIndex = (m_currentHeapIndex + 1) % CELL_HEAP_COUNT;

		LoCell* nextHeap = m_heapBuffer[nextHeapIndex];
		LoCell* buffer = nextHeap;

		for (int i = 0; i < m_env.size(); i++) {
			gc_write(&m_env[i], &buffer);
			LO_ASSERT(m_env[i]->type != LO_CELL_TYPE_FORWARDING);
		}

		for (int i = 0; i < m_stack.size(); i++) {
			gc_write(&m_stack[i], &buffer);
			LO_ASSERT(m_stack[i]->type != LO_CELL_TYPE_FORWARDING);
		}

		for (LoCell* cur = nextHeap; cur != buffer; cur++) {
			if (cur->type == LO_CELL_TYPE_CONS) {
				gc_write(&cur->value.cons_value.car, &buffer);
				gc_write(&cur->value.cons_value.cdr, &buffer);
				LO_ASSERT(cur->type != LO_CELL_TYPE_FORWARDING);
			}
		}

		LO_TRACE("copied: %ld cells", buffer - nextHeap);

		m_currentHeapIndex = nextHeapIndex;
		m_currentHeap = nextHeap;
		m_currentHeapEnd = nextHeap + CELL_HEAP_SIZE;

		for (int j = 0; j < CELL_HEAP_SIZE; j++) {
			m_currentHeap[j].setNil();
		}

	}

	LoCell* apply(LoCell* c)
	{
		LoCell* routine = c->value.cons_value.car;
		LoCell* args = c->value.cons_value.cdr;

		return routine->value.routine_value(this, args);
	}

	static LoCell* add(Lo* lo, LoCell* c)
	{
		// TODO: float
		i32 v = 0;
		while (c->type != LO_CELL_TYPE_NIL) {
			v += c->value.cons_value.car->value.int_value;
			c = c->value.cons_value.cdr;
		}

		c = lo->newCell();
		c->setInt(v);
		return c;
	}

	LoCell* eval(LoCell* c)
	{
		switch (c->type){
			case LO_CELL_TYPE_NIL:
			case LO_CELL_TYPE_INT:
			case LO_CELL_TYPE_FLOAT:
			case LO_CELL_TYPE_ROUTINE:
				return c;

			case LO_CELL_TYPE_SYMBOL:
				// TODO: exception
				return m_env[c->value.symbol_value];

			case LO_CELL_TYPE_CONS:
				// TODO: macro expansion
				{
					LoCell* top = newCell();
					m_stack.push_back(top);
					LoCell* cur = top;
					while (c->type != LO_CELL_TYPE_NIL) {
						LoCell* next = newCell();
						cur->setCons(eval(c->value.cons_value.car), next);
						cur = next;
						c = c->value.cons_value.cdr;
					}
					cur->setNil();
					m_stack.pop_back();
					return apply(top);
				}
		}
	}

	void setEnv(const char* name, LoCell* value)
	{
		i32 i = intern(name);
		if (m_env.size() < i + 1) {
			m_env.resize(i + 1);
		}
		m_env[i] = value;
	}

	void initEnv()
	{
		{
			LoCell* add_op = newCell();
			add_op->setRoutine(add);
			setEnv("+", add_op);
		}

		{
			LoCell* nil = newCell();
			nil->setNil();
			setEnv("nil", nil);
		}
	}


	LoToken pop()
	{
		LoToken front = m_tokens.front();
		m_tokens.pop_front();
		return front;
	}

	void push(LoToken token)
	{
		m_tokens.push_back(token);
	}

	LoCell* read(const char* buf)
	{
		tokenize(buf, m_tokens);
		LoCell* cur = readCell();
		return cur;
	}

	static bool tokenize(const char* pBuf, std::list<LoToken>& tokens)
	{
		while (*pBuf != '\0') {
			char c = *pBuf;
			if (c == ' ' || c == '\n') {
				// tokens.push_back(LoToken(LO_TOKEN_TYPE_SPACE, std::string(1, c)));
				pBuf++;
			} else if (c == '(') {
				tokens.push_back(LoToken(LO_TOKEN_TYPE_LPAREN, std::string(1, c)));
				pBuf++;
			} else if (c == ')') {
				tokens.push_back(LoToken(LO_TOKEN_TYPE_RPAREN, std::string(1, c)));
				pBuf++;
			} else {
				const char* begin = pBuf;
				do {
					pBuf++;
					c = *pBuf;
				} while (c != ' ' && c != '\n' && c != '(' && c != ')');
				tokens.push_back(LoToken(LO_TOKEN_TYPE_ATOM, std::string(begin, pBuf)));
			}
		}
		// tokens.push_back(LoToken(LO_TOKEN_TYPE_EOF, std::string(1, '\0')));
		return true;
	}

	Lo()
	{
		for (int i = 0; i < CELL_HEAP_COUNT; i++) {
			m_heapBuffer[i] = new LoCell[CELL_HEAP_SIZE];
			for (int j = 0; j < CELL_HEAP_SIZE; j++) {
				m_heapBuffer[i][j].setNil();
			}
		}
		m_currentHeapIndex = 0;
		m_currentHeap = m_heapBuffer[m_currentHeapIndex];
		m_currentHeapEnd = m_currentHeap + CELL_HEAP_SIZE;

		initEnv();

		LO_TRACE("Lo initialized. buffer size is %d(cells)", CELL_HEAP_SIZE);
	}

	~Lo()
	{
		for (int i = 0; i < CELL_HEAP_COUNT; i++) {
			delete[] m_heapBuffer[i];
		}
	}

};

#endif // LO_H_INCLUDED

