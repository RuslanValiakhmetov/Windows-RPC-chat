#pragma once

#define THROW_IF(___cond___, ___mess___) if (___cond___) { THROW(___mess___); }
#define THROW(___mess___) {                 \
    std::stringstream ___st;                \
    ___st << ___mess___;                    \
    throw std::runtime_error(___st.str());  \
}

template<typename BindingType, typename DeleterType>
class BindingGuard
{
public:
    BindingGuard() = delete;
    BindingGuard(const BindingGuard&) = delete;
    BindingGuard& operator=(const BindingGuard&) = delete;

    explicit BindingGuard(DeleterType deleter)
        : m_deleter(deleter)
    {
    }

    ~BindingGuard()
    {
        if (m_bindingObject)
        {
            m_deleter(&m_bindingObject);
        }
    }

    BindingType* operator&()
    {
        return &m_bindingObject;
    }

    BindingType operator*()
    {
        return m_bindingObject;
    }

private:
    BindingType m_bindingObject = nullptr;
    DeleterType* m_deleter;
};
