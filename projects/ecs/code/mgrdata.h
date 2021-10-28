#pragma once

#include <vector>

namespace efiilj
{
	struct Extensible
	{ 
		virtual void extend() = 0;
		virtual void pack(std::size_t to, std::size_t from) = 0;
	};

	template<typename U>
	class ComponentDataBase : public Extensible
	{
		protected:

			bool _has_default;
			U _default_value;
			std::vector<U> _data;

		public:

			ComponentDataBase()
				: _has_default(false) 
			{ }

			ComponentDataBase(U default_value)
				: _has_default(true), _default_value(default_value)
			{ }

			void set_default(U value)
			{ _default_value = value; _has_default = true; }

			void pack(std::size_t to, std::size_t from) override
			{ _data[to] = _data[from]; }

			void extend() override
			{ 
				if (_has_default)
					_data.emplace_back(_default_value);
				else
					_data.emplace_back();
			}

			void reset_default(std::size_t idx)
			{ _data[idx] = _default_value; }

			const U& operator [] (std::size_t i) const
			{ return _data[i]; }

			U& operator [] (std::size_t i)
			{ return _data[i]; }
	};

	template<typename U>
	class ComponentData : public ComponentDataBase<U>
	{ 
		public:

			ComponentData()
				: ComponentDataBase<U>()
			{ }

			ComponentData(U default_value)
				: ComponentDataBase<U>(default_value)
			{ }
	};

	template<>
	class ComponentData<bool> : public ComponentDataBase<bool>
	{
		public:

			ComponentData()
				: ComponentDataBase<bool>()
			{ }

			ComponentData(bool default_value)
				: ComponentDataBase<bool>(default_value)
			{ }

			std::vector<bool>::reference operator [] (std::size_t i)
			{ return _data[i]; }

			bool operator [] (std::size_t i) const
			{ return _data[i]; }

	};
}
