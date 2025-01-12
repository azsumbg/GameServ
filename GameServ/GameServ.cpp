#include "pch.h"
#include "GameServ.h"

// RANDiT ************************************

dll::RANDiT::RANDiT()
{
	sq = new std::seed_seq{ std::random_device{}(), std::random_device{}(), std::random_device{}(),
	std::random_device{}(), std::random_device{}(), std::random_device{}(), std::random_device{}(),
	std::random_device{}(), };
	twister = new std::mt19937(*sq);
}

dll::RANDiT::~RANDiT()
{
	delete sq;
	delete twister;
}

int dll::RANDiT::operator() (int min, int max)
{
	std::uniform_int_distribution distrib(min, max);

	return distrib(*twister);
}

//////////////////////////////////////////////

//PROTON ************************************

dll::PROTON::PROTON(float _sx, float _sy, float _width, float _height)
{
	width = _width;
	height = _height;

	myDims.up_left.x = _sx;
	myDims.up_left.y = _sy;

	myDims.up_right.x = _sx + width;
	myDims.up_right.y = _sy;

	myDims.down_left.x = _sx;
	myDims.down_left.y = _sy + height;

	myDims.down_right.x = _sx + width;
	myDims.down_right.y = _sy + height;

	myDims.center.x = _sx + width / 2;
	myDims.center.y = _sy + height / 2;

	myDims.radius = width / 2;

	myDims.col = static_cast<int>(_sx / 30.0f);
	myDims.row = static_cast<int>(_sy / 30.0f);
}
dll::PROTON::PROTON(float _sx, float _sy, float _width, float _height, PROTON* return_pointer)
{
	m_base_ptr = new PROTON(_sx, _sy, _width, _height);
	return_pointer = m_base_ptr;
}
dll::PROTON::~PROTON()
{
	if (m_base_ptr)
	{
		delete m_base_ptr;
		m_base_ptr = nullptr;
	}
}
void dll::PROTON::SetEdges()
{
	myDims.up_right.x = myDims.up_left.x + width;
	myDims.up_right.y = myDims.up_left.y;

	myDims.down_left.x = myDims.up_left.x;
	myDims.down_left.y = myDims.up_left.y + height;

	myDims.down_right.x = myDims.up_left.x + width;
	myDims.down_right.y = myDims.up_left.y + height;

	myDims.center.x = myDims.up_left.x + width / 2;
	myDims.center.y = myDims.up_left.y + height / 2;

	myDims.col = static_cast<int>(myDims.down_left.x / 30.0f);
	myDims.row = static_cast<int>(myDims.down_left.y / 30.0f);
}
void dll::PROTON::NewDims(float new_width, float new_height)
{
	width = new_width;
	height = new_height;
	SetEdges();
}
void dll::PROTON::Move(float to_where_x, float to_where_y)
{
	myDims.up_left.x = to_where_x;
	myDims.up_left.y = to_where_y;
	SetEdges();
}
DIMS dll::PROTON::GetDims() const
{
	return myDims;
}
float dll::PROTON::GetWidth() const
{
	return width;
}
float dll::PROTON::GetHeight() const
{
	return height;
}
dll::PROTON* dll::PROTON::GetHeapAdress() const
{
	return m_base_ptr;
}
void dll::PROTON::SetWidth(float new_width)
{
	width = new_width;
	SetEdges();
}
void dll::PROTON::SetHeight(float new_height)
{
	height = new_height;
	SetEdges();
}

//////////////////////////////////////////////

// BALL *************************************

dll::BALL::BALL(float sx, float sy, float where_x, float where_y, char what_type) :PROTON(sx, sy, 30.0f, 30.0f)
{
	my_type = what_type;

	move_ex = where_x;
	move_ey = where_y;

	if (where_x - sx == 0)vert_line = true;
	if (where_y - (sy + width) == 0)hor_line = true;

	if (!hor_line && !vert_line)
	{
		slope = (where_y - sy) / (where_x / sx);
		intercept = sy - slope * sx;
	}
}
bool dll::BALL::ShootBall(float gear)
{
	if (hor_line)return false;

	float my_speed = speed + gear / 10;

	if (vert_line)
	{
		myDims.up_left.y -= my_speed;
		SetEdges();
		return true;
	}

	if (move_ex > myDims.up_left.x)myDims.up_left.x += my_speed;
	else if (move_ex < myDims.up_left.x)myDims.up_left.x -= my_speed;
	SetEdges();

	if (myDims.up_left.x <= 0)
	{
		myDims.up_left.x = 0;
		SetEdges();
		return false;
	}
	if (myDims.up_right.x >= 300.0f)
	{
		myDims.up_left.x = 270.0f;
		SetEdges();
		return false;
	}
	if (myDims.up_left.y <= 50.0f)
	{
		myDims.up_left.y = 50.0f;
		SetEdges();
		return false;
	}
	if (myDims.down_right.y >= 300.0f)
	{
		myDims.up_right.y = 270.0f;
		SetEdges();
		return false;
	}

	return true;
}
void dll::BALL::FallBall()
{
	myDims.up_left.y += 30.0f;
	SetEdges();
}
void dll::BALL::Release()
{
	delete this;
}
char dll::BALL::GetType() const
{
	return my_type;
}
float dll::BALL::Distance(POINT StartPoint, POINT RefPoint) const
{
	float a = static_cast<float>(pow(abs(RefPoint.x - StartPoint.x), 2));
	float b = static_cast<float>(pow(abs(RefPoint.y - StartPoint.y), 2));

	return sqrt(a + b);
}

/////////////////////////////////////////////

//BALL CONTAINER ****************************

dll::BALLCONTAINER::BALLCONTAINER(size_t lenght) :size{ lenght }, mBasePtr{ new NODE* [lenght] }
{
	for (size_t i = 0; i < size; ++i)(*(mBasePtr + i)) = nullptr;
}
dll::BALLCONTAINER::~BALLCONTAINER()
{
	if (mBasePtr)
	{
		delete[]mBasePtr;
	}
}
void dll::BALLCONTAINER::push_back(NODE* new_node)
{
	if (current_position + 1 <= size)
	{
		(*(mBasePtr + current_position)) = new_node;
		++current_position;
	}
	else
	{
		++size;

		NODE** temp_ptr{ new NODE * [size] };

		for (size_t i = 0; i < size - 1; ++i) (*(temp_ptr + i)) = (*(mBasePtr + i));

		delete[]mBasePtr;

		mBasePtr = temp_ptr;

		*(mBasePtr + (size - 1)) = new_node;
		
		++current_position;
	}

	SetLinks();
}
void dll::BALLCONTAINER::push_at(NODE* new_node, size_t position)
{
	if (position < size)
	{
		*(mBasePtr + position) = new_node;
		SetLinks();
	}
}
void dll::BALLCONTAINER::remove(size_t index)
{
	if (index < size)
	{
		NODE** temp_ptr{ new NODE* [size - 1] };
		
		size_t next_index = 0;

		for (size_t i = 0; i < size; ++i)
			if (i != index)
			{
				*(temp_ptr + next_index) = *(mBasePtr + i);
				next_index++;
			}
			
		delete[]mBasePtr;

		mBasePtr = temp_ptr;
		
		--current_position;
		--size;

		SetLinks();
	}
}
dll::NODE* dll::BALLCONTAINER::operator [](size_t index)
{
	if (index < size)return *(mBasePtr + index);
	return nullptr;
}
size_t dll::BALLCONTAINER::capacity() const
{
	return size;
}
bool dll::BALLCONTAINER::is_valid() const
{
	return size == 0;
}
void dll::BALLCONTAINER::SetLinks()
{
	for (size_t check_node = 0; check_node < size; ++check_node)
	{
		for (size_t nodes = 0; nodes < size; ++nodes)
		{
			if ((*(mBasePtr + check_node)) && (*(mBasePtr + nodes)))
			{
				if (check_node != nodes)
				{
					DIMS CheckNodePoint = (*(mBasePtr + check_node))->mData.GetDims();
					DIMS NodesPoint = (*(mBasePtr + nodes))->mData.GetDims();

					POINT first_point{ (LONG)(CheckNodePoint.center.x),(LONG)(CheckNodePoint.center.y) };
					POINT second_point{ (LONG)(NodesPoint.center.x),(LONG)(NodesPoint.center.y) };

					float dist = (*(mBasePtr + check_node))->mData.Distance(first_point, second_point);
					if (dist <= 60.0f)
					{
						if (first_point.y < second_point.y)(*(mBasePtr + check_node))->m_down_ptr = *(mBasePtr + nodes);
						else if (first_point.y > second_point.y)(*(mBasePtr + check_node))->m_up_ptr = *(mBasePtr + nodes);

						if (first_point.x < second_point.x)(*(mBasePtr + check_node))->m_right_ptr = *(mBasePtr + nodes);
						else if (first_point.x > second_point.x)(*(mBasePtr + check_node))->m_left_ptr = *(mBasePtr + nodes);
					}
				}
			}
		}
	}
}