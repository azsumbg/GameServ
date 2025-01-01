#pragma once

#ifdef GAMESERV_EXPORTS
#define GAMESERV_API __declspec(dllexport)
#else 
#define GAMESERV_API __declspec(dllimport)
#endif

#include <random>

constexpr float scr_width{ 400.0f };
constexpr float scr_height{ 450.0f };

constexpr char blue_ball{ 0b00000001 };
constexpr char red_ball{ 0b00000010 };
constexpr char green_ball{ 0b00000100 };
constexpr char yellow_ball{ 0b00001000 };
constexpr char purple_ball{ 0b00010000 };

struct GAMESERV_API ONE_POINT
{
	float x{ 0 };
	float y{ 0 };
};

struct GAMESERV_API DIMS
{
	 ONE_POINT up_left{ 0, 0 };
	 ONE_POINT up_right{ 0, 0 };
	 ONE_POINT down_left{ 0, 0 };
	 ONE_POINT down_right{ 0, 0 };

	 ONE_POINT center{ 0, 0 };

	 float radius{ 0 };

	 int row{ 0 };
	 int col{ 0 };
};

namespace dll
{
	class GAMESERV_API RANDiT
	{
	private:
		std::seed_seq* sq{ nullptr };
		std::mt19937* twister{ nullptr };

	public:

		RANDiT();
		~RANDiT();

		int operator() (int min, int max);
	};

	class GAMESERV_API PROTON
	{
	protected:

		DIMS myDims{};
		float width{ 0 };
		float height{ 0 };
		PROTON* m_base_ptr{ nullptr };

	public:

		PROTON(float _sx = 0, float _sy = 0, float _width = 1.0f, float _height = 1.0f);
		PROTON(float _sx, float _sy, float _width, float _height, PROTON* return_pointer);

		virtual ~PROTON();

		void SetEdges();
		void NewDims(float new_width, float new_height);

		void Move(float to_where_x, float to_where_y);
		DIMS GetDims() const;

		float GetWidth() const;
		float GetHeight() const;

		PROTON* GetHeapAdress() const;

		void SetWidth(float new_width);
		void SetHeight(float new_height);
	};

	class GAMESERV_API BALL :public PROTON
	{
		protected:
			char my_type{ 0 };
			
			float slope{ 0 };
			float intercept{ 0 };
			
			float move_ex{ 0 };
			float move_ey{ 0 };
			
			float speed = 1.0f;

			bool vert_line = false;
			bool hor_line = false;
			
		public:
			BALL(float sx = 0, float sy = 0, float where_x = 1.0f, float where_y = 1.0f, char what_type = 0);

			bool ShootBall(float gear);
			void FallBall();
			
			char GetType()const;

			void Release();
			float Distance(POINT StartPoint, POINT RefPoint) const;
	};

	struct GAMESERV_API NODE
	{
		BALL* m_base_ptr{ nullptr };
		BALL* m_left_ptr{ nullptr };
		BALL* m_right_ptr{ nullptr };
		BALL* m_up_ptr{ nullptr };
		BALL* m_down_ptr{ nullptr };

		BALL mData{};
	};

	class GAMESERV_API BALLCONTAINER
	{
	private:
		NODE** mBasePtr{ nullptr };

		size_t size = 0;

	public:

		BALLCONTAINER(size_t lenght);
		
		~BALLCONTAINER();

		void push_back(NODE* new_node);
		void push_at(NODE* new_node, size_t position);
		void remove(size_t index);

		size_t capacity() const;
		bool is_valid() const;

		NODE* operator [](size_t index);
	};

}