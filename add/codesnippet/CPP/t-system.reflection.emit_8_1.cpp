using namespace System;
using namespace System::Reflection;
using namespace System::Reflection::Emit;
using namespace System::Globalization;

// Declare a delegate type that can be used to execute the completed
// dynamic method. 
private delegate int HelloDelegate(String^ msg, int ret);

void main()
{
    // Create an array that specifies the types of the parameters
    // of the dynamic method. This dynamic method has a String
    // parameter and an Integer parameter.
    array<Type^>^ helloArgs = { String::typeid, int::typeid };

    // Create a dynamic method with the name "Hello", a return type
    // of Integer, and two parameters whose types are specified by
    // the array helloArgs. Create the method in the module that
    // defines the String class.
    DynamicMethod^ hello = gcnew DynamicMethod("Hello", 
        int::typeid, 
        helloArgs, 
        String::typeid->Module);

    // Create an array that specifies the parameter types of the
    // overload of Console::WriteLine to be used in Hello.
    array<Type^>^ writeStringArgs = { String::typeid };
    // Get the overload of Console::WriteLine that has one
    // String parameter.
    MethodInfo^ writeString = Console::typeid->GetMethod("WriteLine", 
        writeStringArgs);

    // Get an ILGenerator and emit a body for the dynamic method,
    // using a stream size larger than the IL that will be
    // emitted.
    ILGenerator^ il = hello->GetILGenerator(256);
    // Load the first argument, which is a string, onto the stack.
    il->Emit(OpCodes::Ldarg_0);
    // Call the overload of Console::WriteLine that prints a string.
    il->EmitCall(OpCodes::Call, writeString, nullptr);
    // The Hello method returns the value of the second argument;
    // to do this, load the onto the stack and return.
    il->Emit(OpCodes::Ldarg_1);
    il->Emit(OpCodes::Ret);

    // Add parameter information to the dynamic method. (This is not
    // necessary, but can be useful for debugging.) For each parameter,
    // identified by position, supply the parameter attributes and a 
    // parameter name.
    hello->DefineParameter(1, ParameterAttributes::In, "message");
    hello->DefineParameter(2, ParameterAttributes::In, "valueToReturn");

    // Create a delegate that represents the dynamic method. This
    // action completes the method. Any further attempts to
    // change the method are ignored.
    HelloDelegate^ hi = 
        (HelloDelegate^) hello->CreateDelegate(HelloDelegate::typeid);

    // Use the delegate to execute the dynamic method.
    Console::WriteLine("\r\nUse the delegate to execute the dynamic method:");
    int retval = hi("\r\nHello, World!", 42);
    Console::WriteLine("Invoking delegate hi(\"Hello, World!\", 42) returned: " + retval);

    // Execute it again, with different arguments.
    retval = hi("\r\nHi, Mom!", 5280);
    Console::WriteLine("Invoking delegate hi(\"Hi, Mom!\", 5280) returned: " + retval);

    Console::WriteLine("\r\nUse the Invoke method to execute the dynamic method:");
    // Create an array of arguments to use with the Invoke method.
    array<Object^>^ invokeArgs = { "\r\nHello, World!", 42 };
    // Invoke the dynamic method using the arguments. This is much
    // slower than using the delegate, because you must create an
    // array to contain the arguments, and value-type arguments
    // must be boxed.
    Object^ objRet = hello->Invoke(nullptr, BindingFlags::ExactBinding, nullptr, invokeArgs, gcnew CultureInfo("en-us"));
    Console::WriteLine("hello.Invoke returned: " + objRet);

    Console::WriteLine("\r\n ----- Display information about the dynamic method -----");
    // Display MethodAttributes for the dynamic method, set when 
    // the dynamic method was created.
    Console::WriteLine("\r\nMethod Attributes: {0}", hello->Attributes);

    // Display the calling convention of the dynamic method, set when the 
    // dynamic method was created.
    Console::WriteLine("\r\nCalling convention: {0}", hello->CallingConvention);

    // Display the declaring type, which is always null for dynamic
    // methods.
    if (hello->DeclaringType == nullptr)
    {
        Console::WriteLine("\r\nDeclaringType is always null for dynamic methods.");
    }
    else
    {
        Console::WriteLine("DeclaringType: {0}", hello->DeclaringType);
    }

    // Display the default value for InitLocals.
    if (hello->InitLocals)
    {
        Console::Write("\r\nThis method contains verifiable code.");
    }
    else
    {
        Console::Write("\r\nThis method contains unverifiable code.");
    }
    Console::WriteLine(" (InitLocals = {0})", hello->InitLocals);

    // Display the module specified when the dynamic method was created.
    Console::WriteLine("\r\nModule: {0}", hello->Module);

    // Display the name specified when the dynamic method was created.
    // Note that the name can be blank.
    Console::WriteLine("\r\nName: {0}", hello->Name);

    // For dynamic methods, the reflected type is always null.
    if (hello->ReflectedType == nullptr)
    {
        Console::WriteLine("\r\nReflectedType is null.");
    }
    else
    {
        Console::WriteLine("\r\nReflectedType: {0}", hello->ReflectedType);
    }

    if (hello->ReturnParameter == nullptr)
    {
        Console::WriteLine("\r\nMethod has no return parameter.");
    }
    else
    {
        Console::WriteLine("\r\nReturn parameter: {0}", hello->ReturnParameter);
    }

    // If the method has no return type, ReturnType is System.Void.
    Console::WriteLine("\r\nReturn type: {0}", hello->ReturnType);

    // ReturnTypeCustomAttributes returns an ICustomeAttributeProvider
    // that can be used to enumerate the custom attributes of the
    // return value. At present, there is no way to set such custom
    // attributes, so the list is empty.
    if (hello->ReturnType == Void::typeid)
    {
        Console::WriteLine("The method has no return type.");
    }
    else
    {
        ICustomAttributeProvider^ caProvider = hello->ReturnTypeCustomAttributes;
        array<Object^>^ returnAttributes = caProvider->GetCustomAttributes(true);
        if (returnAttributes->Length == 0)
        {
            Console::WriteLine("\r\nThe return type has no custom attributes.");
        }
        else
        {
            Console::WriteLine("\r\nThe return type has the following custom attributes:");
            for each (Object^ attr in returnAttributes)
            {
                Console::WriteLine("\t{0}", attr->ToString());
            }
        }
    }

    Console::WriteLine("\r\nToString: {0}", hello->ToString());

    // Display parameter information.
    array<ParameterInfo^>^ parameters = hello->GetParameters();
    Console::WriteLine("\r\nParameters: name, type, ParameterAttributes");
    for each (ParameterInfo^ p in parameters)
    {
        Console::WriteLine("\t{0}, {1}, {2}", 
            p->Name, p->ParameterType, p->Attributes);
    }
}

/* This code example produces the following output:

Use the delegate to execute the dynamic method:

Hello, World!
Invoking delegate hi("Hello, World!", 42) returned: 42

Hi, Mom!
Invoking delegate hi("Hi, Mom!", 5280) returned: 5280

Use the Invoke method to execute the dynamic method:

Hello, World!
hello.Invoke returned: 42

 ----- Display information about the dynamic method -----

Method Attributes: PrivateScope, Public, Static

Calling convention: Standard

DeclaringType is always null for dynamic methods.

This method contains verifiable code. (InitLocals = True)

Module: CommonLanguageRuntimeLibrary

Name: Hello

ReflectedType is null.

Method has no return parameter.

Return type: System.Int32

The return type has no custom attributes.

ToString: Int32 Hello(System.String, Int32)

Parameters: name, type, ParameterAttributes
        message, System.String, In
        valueToReturn, System.Int32, In
 */