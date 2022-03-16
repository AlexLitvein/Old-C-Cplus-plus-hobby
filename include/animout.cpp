#include "animout.h"

void PrintMatrixToLogFile(HANDLE file, Matrix3 &m)
{
	Write_Log_String(ghLog, "\n");
	int i = 0;	
    for( ; i <= 3; ++i)
    {
		Point3 pt = m.GetRow(i);
		Write_Log_String(ghLog, " %9f %9f %9f\n", pt.x, pt.y, pt.z);
    }
	Write_Log_String(ghLog, "\n");
}


// Чтобы ф-ия при прочих условия экспортировала анимацию, надо чтобы
// объект транлировался целиком(как например "кость"), а не отдельные
// его вершины.
void ExportAnimKeys(int stTick, int endTick, INode* node) 
{	
	/*BOOL bPosAnim;
	BOOL bRotAnim;
	BOOL bScaleAnim;*/
	BOOL  bDoKeys = FALSE;
	DWORD byteWriten;

	// Мы можем экспортировать ключи, только если все контроллеры ТМ нам "известны".
	// причина в том, что некоторые контроллеры управляют больше чем какой{что}
	// они должны. Рассмотрим контроллер позиции пути, если Вы включаете
	// следует и банковское дело, этот контроллер позиции будет также управлять
	// вращением. Если узел, который имел контроллер позиции пути также, имел
	// TCB контроллер вращения, TCB ключи не описали бы целое вращение узла.
	// По этой причине мы только экспортируем ключи если все контроллеры
	// позиция, вращение и масштаб являются линейными, гибридными (bezier) или tcb.

	if (!useSample)
	{
		Write_Log_String(ghLog, "Хотим экспортировать ключи.\n");
		Control* pC = node->GetTMController()->GetPositionController();
		Control* rC = node->GetTMController()->GetRotationController();
		Control* sC = node->GetTMController()->GetScaleController();

		if (IsKnownController(pC) && IsKnownController(rC) && IsKnownController(sC)) bDoKeys = TRUE;
	}
	if (bDoKeys) 
	{
		Write_Log_String(ghLog, "Найден известный контроллер, ключи будут экспортированы.\n");
		// Only dump the track header if any of the controllers have keys
		if (node->GetTMController()->GetPositionController()->NumKeys() ||
			node->GetTMController()->GetRotationController()->NumKeys() ||
			node->GetTMController()->GetScaleController()->NumKeys()) 
		{
			//Write_Log_String(ghLog, "%s\n", ID_TM_ANIMATION);
			//fprintf(pStream,"%s\t%s {\n", indent.data(), ID_TM_ANIMATION); 
			//Write_Log_String(ghLog, "%s: %s\n", ID_NODE_NAME, node->GetName());
			//fprintf(pStream,"%s\t\t%s \"%s\"\n", indent.data(), ID_NODE_NAME, FixupName(node->GetName()));

			DumpPosKeys(node->GetTMController()->GetPositionController());
			DumpRotKeys(node->GetTMController()->GetRotationController());
			DumpScaleKeys(node->GetTMController()->GetScaleController());
		}
	}
	else 
	{		
		float delta = GetTicksPerFrame() * gKeyFrameStep;		
		for ( ; stTick <= endTick; stTick += delta) 
		{			
			//D3DXMATRIX objectTM;						
			D3DXMATRIX final;

			/*D3DXMATRIX swap;
			swap._11 =  1.0f; swap._12 =  0.0f; swap._13 =  0.0f; swap._14 = 0.0f;
			swap._21 =  0.0f; swap._22 =  0.0f; swap._23 = -1.0f;  swap._24 = 0.0f;
			swap._31 =  0.0f; swap._32 =  1.0f; swap._33 =  0.0f; swap._34 = 0.0f;
			swap._41 =  0.0f; swap._42 =  0.0f; swap._43 =  0.0f; swap._44 = 1.0f;*/
			
			/*SetDXMatrix(nodeTM, node->GetNodeTM(stTick));
			SetDXMatrix(nodeTM0, node->GetNodeTM(0));
			SetDXMatrix(parentTM, node->GetParentTM(stTick));	
			SetDXMatrix(parentTM0, node->GetParentTM(0));*/

			SetDXMatrix(final, node->GetObjectTM(stTick));
//================================================================
			Write_Log_String(ghLog, "=============================ExportAnimKeys==================================\n");
			
			/*Write_Log_String(ghLog, "objectTM->");
			PrintD3DXMATRIXToLogFile(ghLog, objectTM);*/

			//final = objectTM * swap;

			/*D3DXVECTOR3		OutScale;
			D3DXQUATERNION	OutRotation;
			D3DXVECTOR3		OutTranslation;	
			D3DXVECTOR3		axis;	
			float			angle;
			D3DXMATRIX		outRot;	*/		
			
			//D3DXMatrixDecompose(&OutScale, &OutRotation, &OutTranslation, &final);
			//D3DXQuaternionToAxisAngle(&OutRotation, &axis, &angle);

			/*Write_Log_String(ghLog, "OutRotation: %f %f %f %f\n", OutRotation.x, OutRotation.y, OutRotation.z, OutRotation.w);
			Write_Log_String(ghLog, "OutTranslation: %f %f %f %f\n", OutTranslation.x, OutTranslation.y, OutTranslation.z); 
			Write_Log_String(ghLog, "axis: %f %f %f angle: %f\n", axis.x, axis.y, axis.z, angle); 
			*/
									
//========================================			
			WriteFile(ghFile, final, sizeof(D3DXMATRIX), &byteWriten, 0);
			Write_Log_String(ghLog, "time: %d\n", stTick);
			PrintD3DXMATRIXToLogFile(ghLog, final);					
		} 		
	}
}

// Чтобы действительно видеть, анимирован ли узел, мы можем шагнуть через диапазон анимации
// и анализируйте{расчленяйте} матрицу ТМ для каждого кадра, и исследовать компоненты.
// Таким образом мы можем идентифицировать анимацию: позиции, вращения и масштабирования отдельно.
// 
// Некоторые контроллеры делают проблематичным, проверку TMContollers вместо
// фактический TMMatrix. Например, контроллер пути - контроллер позиции, 
// но если Вы включаете, следуют и банковское дело, это также затронет компонент вращения.
// Если мы хотим к, мы можем исследовать позицию, вращение и масштабировать контроллеры и
// если они все являются Линейными, Гибридными (bezier) или TCB, то мы могли экспортировать фактические ключи.
// Это нисколько не трудно, но импортер должен знать точную интерполяцию
// алгоритм, чтобы использовать это. Исходный текст к процедурам интерполяции доступен
// ADN членам.
// 
// Для примера того, как экспортировать фактические ключи, смотрите на DumpPoint3Keys () ниже.
// Этот метод проверит фактический контроллер, чтобы определить, известен ли контроллер.
// Если мы знаем, как работает, этот контроллер, его фактические ключи экспортируется,
// иначе контроллер будет выборка, используя пользовательскую частоту осуществления выборки.

// Функция возвращает true если хоть один контроллер анимации делает свою работу,
// и устанавливает входные переменые в true если СООТВЕТСТВУЮЩИЙ контроллер анимации делает свою работу.
BOOL CheckForAnimation(INode* node, BOOL& bPos, BOOL& bRot, BOOL& bScale)
{
	int start = giPtr->GetAnimRange().Start();
	int end = giPtr->GetAnimRange().End();
	int t;
	int delta = GetTicksPerFrame();
	Matrix3 tm;
	AffineParts ap;
	Point3 firstPos;
	float rotAngle, firstRotAngle;
	Point3 rotAxis, firstRotAxis;
	Point3 firstScaleFactor;

	bPos = bRot = bScale = FALSE;

	for (t=start; t<=end; t+=delta) 
	{
		tm = node->GetNodeTM(t) * Inverse(node->GetParentTM(t));
		decomp_affine(tm, &ap);
		AngAxisFromQ(ap.q, &rotAngle, rotAxis);

		if (t != start) 
		{
			if (!bPos) 
			{
				if (!EqualPoint3(ap.t, firstPos)) bPos = TRUE;
			}
			// MAX 2.x:
			// Мы исследуем угол вращения, чтобы видеть если компонент вращения изменился.
			// Хотя entierly не верный, это должно работать.
			// Редко, когда ось вращения анимируеися без изменения угла вращения.
			// MAX 3.x:
			// Вышеупомянутое не работет, если мы имеем сцену, которая не экспортирует дорожку вращения.
			// Я установил это, чтобы также сравнить ось.
			if (!bRot) 
			{
				if (fabs(rotAngle - firstRotAngle) > ALMOST_ZERO) bRot = TRUE;
				else if (!EqualPoint3(rotAxis, firstRotAxis)) bRot = TRUE;
			}

			if (!bScale) 
			{
				if (!EqualPoint3(ap.k, firstScaleFactor)) bScale = TRUE;
			}
		}
		else 
		{
			firstPos = ap.t;
			firstRotAngle = rotAngle;
			firstRotAxis = rotAxis;
			firstScaleFactor = ap.k;
		}

		// Не требуется продолжать цикл если все сомпоненты анимированы
		if (bPos && bRot && bScale) break;
	}

	return bPos || bRot || bScale;
}

void DumpPosSample(INode* node) 
{
	//fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_POS_TRACK);
	Write_Log_String(ghLog, "%s\n", ID_POS_TRACK);

	int start = giPtr->GetAnimRange().Start();
	int end = giPtr->GetAnimRange().End();	
	int t;
	float delta = GetTicksPerFrame() * gKeyFrameStep;
	Write_Log_String(ghLog, "GetAnimRange %d,  GetTicksPerFrame() * gKeyFrameStep: %d\n", end, delta);
	Matrix3 tm;
	AffineParts ap;
	Point3	prevPos;

	for (t=start; t<=end; t+=delta) 
	{
		tm = node->GetNodeTM(t) * Inverse(node->GetParentTM(t)); // переход в локальную систему координат кости
		decomp_affine(tm, &ap);

		Point3 pos = ap.t;
		if (t!= start && EqualPoint3(pos, prevPos)) 
		{
			// Skip identical keys 
			continue;
		}
		prevPos = pos;

		// Output the sample
		//fprintf(pStream, "%s\t\t\t%s %d\t%s\n", indent.data(), ID_POS_SAMPLE, t, Format(pos));
		Write_Log_String(ghLog, "%s время: %d Point3 pos: %f %f %f\n", ID_POS_SAMPLE, t, pos.x, pos.y, pos.z);
	}
}


void DumpRotSample(INode* node) 
{		
	//fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_ROT_TRACK);
	Write_Log_String(ghLog, "%s\n", ID_ROT_TRACK);

	int start = giPtr->GetAnimRange().Start();
	int end = giPtr->GetAnimRange().End();	
	int t;
	float delta = GetTicksPerFrame() * gKeyFrameStep;
	Write_Log_String(ghLog, "GetAnimRange %d,  GetTicksPerFrame() * gKeyFrameStep: %d\n", end, delta);
	Matrix3 tm;
	AffineParts ap;
	Quat prevQ;

	prevQ.Identity();

	for (t=start; t<=end; t+=delta) 
	{
		tm = node->GetNodeTM(t) * Inverse(node->GetParentTM(t));
		decomp_affine(tm, &ap);

		// Rotation keys should be relative, so we need to convert these
		// absolute samples to relative values.

		Quat q = ap.q / prevQ;
		prevQ = ap.q;

		if (q.IsIdentity()) 
		{
			// No point in exporting null keys...
			continue;
		}
		// Output the sample
		Point3 axis;
		float angle;
		AngAxisFromQ(q, &angle, axis);
		//fprintf(pStream, "%s\t\t\t%s %d\t%s\n",	indent.data(), ID_ROT_SAMPLE, t, Format(q));
		Write_Log_String(ghLog, "%s время: %d Quat q(axis/angle): %f %f %f %f\n", 
								ID_POS_SAMPLE, t, axis.x, axis.y, axis.z, angle);
	}
}

void DumpScaleSample(INode* node) 
{	
	//fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_SCALE_TRACK);
	Write_Log_String(ghLog, "%s\n", ID_SCALE_TRACK);

	int start = giPtr->GetAnimRange().Start();
	int end = giPtr->GetAnimRange().End();
	Write_Log_String(ghLog, "GetAnimRange %d\n", end);
	int t;
	float delta = GetTicksPerFrame() * gKeyFrameStep;
	Matrix3 tm;
	AffineParts ap;
	Point3	prevFac;

	for (t=start; t<=end; t+=delta) 
	{
		tm = node->GetNodeTM(t) * Inverse(node->GetParentTM(t));
		decomp_affine(tm, &ap);

		if (t!= start && EqualPoint3(ap.k, prevFac)) 
		{
			// Skip identical keys 
			continue;
		}

		prevFac = ap.k;

		// Output the sample
		/*fprintf(pStream, "%s\t\t\t%s %d\t%s %s\n",indent.data(),
			ID_SCALE_SAMPLE, t,	Format(ap.k), Format(ap.u));*/
		Write_Log_String(ghLog, "%s время: %d Stretch factors: %f %f %f Stretch rotation: %f %f %f\n", 
						ID_SCALE_SAMPLE, t, ap.k.x, ap.k.y, ap.k.z, 
											ap.u.x, ap.u.y, ap.u.z);
	}
}

// Output point3 keys if this is a known point3 controller that
// supports key operations. Otherwise we will sample the controller 
// once for each frame to get the value.
// Point3 controllers can control, for example, color.
void DumpPoint3Keys(Control* cont) 
{
	if (!cont)	// Bug out if no controller.
		return;
	
	int i;
	//TSTR indent = GetIndent(indentLevel);
	IKeyControl *ikc = NULL;

	// If the user wants us to always sample, we will ignore the KeyControlInterface
	if (!useSample) ikc = GetKeyControlInterface(cont);
	
	// TCB point3
	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_POINT3_CLASS_ID, 0)) 
	{
		//fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_POINT3_TCB); 
		Write_Log_String(ghLog, "%s\n", ID_CONTROL_POINT3_TCB);
		for (i=0; i<ikc->GetNumKeys(); i++) 
		{
			ITCBPoint3Key key;
			ikc->GetKey(i, &key);
			/*fprintf(pStream, "%s\t\t\t%s %d\t%s",indent.data(),ID_TCB_POINT3_KEY,key.time,Format(key.val));*/
			Write_Log_String(ghLog, "%s время: %d Point3 val: %f %f %f\n", 
						ID_SCALE_SAMPLE, key.time, key.val.x, key.val.y, key.val.z);
			// Add TCB specific data
			//fprintf(pStream, "\t%s\t%s\t%s\t%s\t%s\n", Format(key.tens), Format(key.cont), Format(key.bias), Format(key.easeIn), Format(key.easeOut));
			Write_Log_String(ghLog, "ITCBPoint3Key key.tens: %f, ITCBPoint3Key key.bias: %f, ITCBPoint3Key key.easeIn: %f, ITCBPoint3Key key.easeOut: %f\n", 
													key.tens, key.bias, key.easeIn, key.easeOut);
		}
	}
	// Bezier point3
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_POINT3_CLASS_ID, 0)) 
	{
		//fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_POINT3_BEZIER); 
		//Write_Log_String(ghLog, "%s\n", ID_CONTROL_POINT3_BEZIER);
		for (i=0; i<ikc->GetNumKeys(); i++) 
		{
			IBezPoint3Key key;
			ikc->GetKey(i, &key);
			/*fprintf(pStream, "%s\t\t\t%s %d\t%s",
				indent.data(),
				ID_BEZIER_POINT3_KEY,
				key.time, 
				Format(key.val));
			fprintf(pStream, "\t%s\t%s\t%d\n", Format(key.intan), Format(key.outtan), key.flags);*/
			Write_Log_String(ghLog, "IBezPoint3Key key.intan: %f %f %f, key.outtan: %f %f %f key.flags: %d\n", 
													key.intan.x, key.intan.y, key.intan.z, 
													key.outtan.x, key.outtan.y, key.outtan.z,
													key.flags);
		}
	}
	// Bezier color
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_COLOR_CLASS_ID, 0)) 
	{
		//fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_COLOR_BEZIER); 
		//Write_Log_String(ghLog, "%s\n", ID_CONTROL_COLOR_BEZIER);

		for (i=0; i<ikc->GetNumKeys(); i++) 
		{
			IBezPoint3Key key;
			ikc->GetKey(i, &key);
			/*fprintf(pStream, "%s\t\t\t%s %d\t%s", 
				indent.data(),
				ID_BEZIER_POINT3_KEY,
				key.time,
				Format(key.val));
			fprintf(pStream, "\t%s\t%s\t%d\n", Format(key.intan), Format(key.outtan), key.flags);*/
			Write_Log_String(ghLog, "IBezPoint3Key key.intan: %f %f %f, key.outtan: %f %f %f key.flags: %d\n", 
													key.intan.x, key.intan.y, key.intan.z, 
													key.outtan.x, key.outtan.y, key.outtan.z,
													key.flags);
		}
	}
	else 
	{		
		// Unknown controller, no key interface or sample on demand -
		// This might be a procedural controller or something else we
		// don't know about. The last resort is to get the value from the 
		// controller at every n frames.
		
		TSTR name;
		cont->GetClassName(name);
		//fprintf(pStream,"%s\t\t%s \"%s\" {\n", indent.data(), ID_CONTROL_POINT3_SAMPLE, FixupName(name));
		//Write_Log_String(ghLog, "%s name: %s\n", ID_CONTROL_POINT3_SAMPLE, name);
		
		// If it is animated at all...
		if (cont->IsAnimated()) 
		{
			// Get the range of the controller animation 
			Interval range; 
			// Get range of full animation
			Interval animRange = giPtr->GetAnimRange(); 
			int t = cont->GetTimeRange(TIMERANGE_ALL).Start();
			Point3 value;
			
			// While we are inside the animation... 
			while (animRange.InInterval(t)) 
			{
				// Sample the controller
				range = FOREVER;
				cont->GetValue(t, &value, range);
				
				// Set time to start of controller validity interval 
				t = range.Start();
				
				// Output the sample
				/*fprintf(pStream, "%s\t\t\t%s %d\t%s\n",
					indent.data(),
					ID_POINT3_KEY,
					t,
					Format(value));*/
				Write_Log_String(ghLog, "%s время: %d value: %f %f %f\n", 
										ID_POINT3_KEY, t, value.x, value.y, value.z);
				
				// If the end of the controller validity is beyond the 
				// range of the animation
				if (range.End() > cont->GetTimeRange(TIMERANGE_ALL).End()) break;
				else t = (range.End()/GetTicksPerFrame()+gKeyFrameStep) * GetTicksPerFrame();
			}
		}
	}
}

// Output float keys if this is a known float controller that
// supports key operations. Otherwise we will sample the controller 
// once for each frame to get the value.
void DumpFloatKeys(Control* cont) 
{
	if (!cont)
		return;
	
	int i;
	//TSTR indent = GetIndent(indentLevel);
	IKeyControl *ikc = NULL;

	// If the user wants us to always sample, we will ignore the KeyControlInterface
	if (!useSample)
		ikc = GetKeyControlInterface(cont);
	
	// TCB float
	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_FLOAT_CLASS_ID, 0)) 
	{
		//fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_FLOAT_TCB); 
		Write_Log_String(ghLog, "%s name: %s\n", ID_CONTROL_FLOAT_TCB);

		for (i=0; i<ikc->GetNumKeys(); i++) 
		{
			ITCBFloatKey key;
			ikc->GetKey(i, &key);
			/*fprintf(pStream, "%s\t\t\t%s %d\t%s",
				indent.data(),
				ID_TCB_FLOAT_KEY,
				key.time,
				Format(key.val));*/
			Write_Log_String(ghLog, "%s время: %d key.val: %f\n", ID_TCB_FLOAT_KEY, key.time, key.val);
			//fprintf(pStream, "\t%s\t%s\t%s\t%s\t%s\n", Format(key.tens), Format(key.cont), Format(key.bias), Format(key.easeIn), Format(key.easeOut));*/
			Write_Log_String(ghLog, "ITCBFloatKey key.tens: %f, key.cont: %f key.bias: %f key.easeIn: %f key.easeOut: %f\n", 
									key.tens, key.cont, key.bias, key.easeIn, key.easeOut);
		}
	}
	// Bezier float
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_FLOAT_CLASS_ID, 0))
	{
		//fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_FLOAT_BEZIER); 
		Write_Log_String(ghLog, "%s name: %s\n", ID_CONTROL_FLOAT_BEZIER);
		for (i=0; i<ikc->GetNumKeys(); i++) 
		{
			IBezFloatKey key;
			ikc->GetKey(i, &key);
			/*fprintf(pStream, "%s\t\t\t%s %d\t%s",
				indent.data(),
				ID_BEZIER_FLOAT_KEY,
				key.time, 
				Format(key.val));*/
			Write_Log_String(ghLog, "%s время: %d key.val: %f\n", ID_BEZIER_FLOAT_KEY, key.time, key.val);

			//fprintf(pStream, "\t%s\t%s\t%d\n", Format(key.intan), Format(key.outtan), key.flags);*/
			Write_Log_String(ghLog, "IBezFloatKey key.intan: %f, key.outtan: %f key.flags: %d\n", 
													key.intan, key.outtan, key.flags);
		}
	}
	else if (ikc && cont->ClassID() == Class_ID(LININTERP_FLOAT_CLASS_ID, 0)) 
	{
		//fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_FLOAT_LINEAR); 
		Write_Log_String(ghLog, "%s\n", ID_CONTROL_FLOAT_LINEAR);

		for (i=0; i<ikc->GetNumKeys(); i++) 
		{
			ILinFloatKey key;
			ikc->GetKey(i, &key);
			/*fprintf(pStream, "%s\t\t\t%s %d\t%s\n",
				indent.data(),
				ID_FLOAT_KEY,
				key.time,
				Format(key.val));*/
			Write_Log_String(ghLog, "%s время: %d key.val: %f\n", ID_FLOAT_KEY, key.time, key.val);
		}
	}
	else 
	{		
		// Unknown controller, no key interface or sample on demand -
		// This might be a procedural controller or something else we
		// don't know about. The last resort is to get the value from the 
		// controller at every n frames.
		
		TSTR name;
		cont->GetClassName(name);
		//fprintf(pStream,"%s\t\t%s \"%s\" {\n", indent.data(), ID_CONTROL_FLOAT_SAMPLE,FixupName(name));
		Write_Log_String(ghLog, "%s name: %s\n", ID_CONTROL_FLOAT_SAMPLE, name);
		
		// If it is animated at all...
		if (cont->IsAnimated()) 
		{
			// Get the range of the controller animation 
			Interval range; 
			// Get range of full animation
			Interval animRange = giPtr->GetAnimRange(); 
			int t = cont->GetTimeRange(TIMERANGE_ALL).Start();
			float value;
			
			// While we are inside the animation... 
			while (animRange.InInterval(t)) 
			{
				// Sample the controller
				range = FOREVER;
				cont->GetValue(t, &value, range);
				
				// Set time to start of controller validity interval 
				t = range.Start();
				
				// Output the sample
				/*fprintf(pStream, "%s\t\t\t%s %d\t%s\n",
					indent.data(),
					ID_FLOAT_KEY,
					t,
					Format(value));*/
				Write_Log_String(ghLog, "%s время: %d value: %f\n", ID_FLOAT_KEY, t, value);
				
				// If the end of the controller validity is beyond the 
				// range of the animation
				if (range.End() > cont->GetTimeRange(TIMERANGE_ALL).End()) break;
				else t = (range.End()/GetTicksPerFrame()+gKeyFrameStep) * GetTicksPerFrame();
			}
		}
	}
}


void DumpPosKeys(Control* cont) 
{
	if (!cont) return;
	
	int i;
	//TSTR indent = GetIndent(indentLevel);
	IKeyControl *ikc = GetKeyControlInterface(cont);
	
	// TCB position
	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_POSITION_CLASS_ID, 0)) 
	{
		int numKeys;
		if (numKeys = ikc->GetNumKeys()) 
		{
			//fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_POS_TCB); 
			Write_Log_String(ghLog, "%s\n", ID_CONTROL_POS_TCB);

			for (i=0; i<numKeys; i++) 
			{
				ITCBPoint3Key key;
				ikc->GetKey(i, &key);
				/*fprintf(pStream, "%s\t\t\t%s %d\t%s",
					indent.data(),
					ID_TCB_POS_KEY,
					key.time,
					Format(key.val));*/
				Write_Log_String(ghLog, "%s время: %d key.val: %f\n", ID_TCB_POS_KEY, key.time, key.val);

				//fprintf(pStream, "\t%s\t%s\t%s\t%s\t%s\n", Format(key.tens), Format(key.cont), Format(key.bias), Format(key.easeIn), Format(key.easeOut));*/
				Write_Log_String(ghLog, "ITCBPoint3Key key.tens: %f, key.cont: %f key.bias: %f key.easeIn: %f key.easeOut: %f\n", 
									key.tens, key.cont, key.bias, key.easeIn, key.easeOut);
			}
		}
	}
	// Bezier position
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_POSITION_CLASS_ID, 0))
	{
		int numKeys;
		if(numKeys = ikc->GetNumKeys())
		{
			//fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_POS_BEZIER); 
			Write_Log_String(ghLog, "%s\n", ID_CONTROL_POS_BEZIER);

			for (i=0; i<numKeys; i++) 
			{
				IBezPoint3Key key;
				ikc->GetKey(i, &key);
				/*fprintf(pStream, "%s\t\t\t%s %d\t%s",
					indent.data(), 
					ID_BEZIER_POS_KEY,
					key.time,
					Format(key.val));*/
				Write_Log_String(ghLog, "%s время: %d key.val: %f\n", ID_BEZIER_POS_KEY, key.time, key.val);

				//fprintf(pStream, "\t%s\t%s\t%d\n", Format(key.intan), Format(key.outtan), key.flags);*/
				Write_Log_String(ghLog, "IBezPoint3Key key.intan: %f %f %f, key.outtan: %f %f %f key.flags: %d\n", 
													key.intan.x, key.intan.y, key.intan.z, 
													key.outtan.x, key.outtan.y, key.outtan.z,
													key.flags);
			}
		}
	}
	// Linear position
	else if (ikc && cont->ClassID() == Class_ID(LININTERP_POSITION_CLASS_ID, 0)) 
	{
		int numKeys;
		if(numKeys = ikc->GetNumKeys())
		{
			//fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_POS_LINEAR); 
			Write_Log_String(ghLog, "%s\n", ID_CONTROL_POS_LINEAR);
			for (i=0; i<numKeys; i++) 
			{
				ILinPoint3Key key;
				ikc->GetKey(i, &key);
				/*fprintf(pStream, "%s\t\t\t%s %d\t%s\n",
					indent.data(),
					ID_POS_KEY,
					key.time,
					Format(key.val));*/
				Write_Log_String(ghLog, "%s время: %d key.val: %f %f %f\n", ID_POS_KEY, key.time, key.val.x, key.val.y, key.val.z);
			}
		}
	}
}

void DumpRotKeys(Control* cont) 
{
	//DWORD byteWriten;
	if (!cont) return;
	
	int i;	
	IKeyControl *ikc = GetKeyControlInterface(cont);	
	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_ROTATION_CLASS_ID, 0)) 
	{
		int numKeys;
		if (numKeys = ikc->GetNumKeys()) 
		{			
			Write_Log_String(ghLog, "%s\n", ID_CONTROL_ROT_TCB);
			for (i=0; i<numKeys; i++) 
			{
				ITCBRotKey key;
				ikc->GetKey(i, &key);
				D3DXQUATERNION q;
				q.x = key.val.axis.x; q.y = key.val.axis.y; q.z = key.val.axis.z; q.w = key.val.angle; 

				//WriteFile(ghFile, q, sizeof(D3DXQUATERNION), &byteWriten, 0);		
				Write_Log_String(ghLog, "%s время: %d key.val.axis: %f %f %f key.val.angle: %f\n", 
										ID_TCB_ROT_KEY, key.time, 
										key.val.axis.x, key.val.axis.y, key.val.axis.z, 
										key.val.angle);
//------------------------------------------------------------------------
				/*Matrix3 m;
				m.SetAngleAxis(key.val.axis, key.val.angle);
				PrintMatrixToLogFile(ghLog, m);*/


//---------------------------------------------------------------------------				
				/*Write_Log_String(ghLog, "ITCBRotKey key.tens: %f, key.cont: %f key.bias: %f key.easeIn: %f key.easeOut: %f\n", 
									key.tens, key.cont, key.bias, key.easeIn, key.easeOut);*/
			}
		}
	}
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_ROTATION_CLASS_ID, 0))
	{
		int numKeys;
		if (numKeys = ikc->GetNumKeys()) 
		{
			//fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_ROT_BEZIER); 
			Write_Log_String(ghLog, "%s\n", ID_CONTROL_ROT_BEZIER);

			for (i=0; i<numKeys; i++) 
			{
				IBezQuatKey key;
				ikc->GetKey(i, &key);
				//fprintf(pStream, "%s\t\t\t%s %d\t%s\n", 
				//	indent.data(),
				//	 // Quaternions are converted to AngAxis when written to file
				//	ID_ROT_KEY,
				//	key.time,
				//	Format(key.val));				
				Write_Log_String(ghLog, "%s время: %d key.val: %f %f %f w=%f\n", 
										ID_ROT_KEY, key.time, key.val.x, key.val.y, key.val.z, key.val.w);
				// There is no intan/outtan for Quat Rotations
				
			}
		}
	}
	else if (ikc && cont->ClassID() == Class_ID(LININTERP_ROTATION_CLASS_ID, 0)) {
		int numKeys;
		if (numKeys = ikc->GetNumKeys()) 
		{
			//fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_ROT_LINEAR);
			Write_Log_String(ghLog, "%s\n", ID_CONTROL_ROT_LINEAR);

			for (i=0; i<numKeys; i++) 
			{
				ILinRotKey key;
				ikc->GetKey(i, &key);
				//fprintf(pStream, "%s\t\t\t%s %d\t%s\n",
				//	indent.data(), 
				//	 // Quaternions are converted to AngAxis when written to file
				//	ID_ROT_KEY,
				//	key.time,
				//	Format(key.val));				
				Write_Log_String(ghLog, "%s время: %d key.val: %f %f %f w=%f\n", 
										ID_ROT_KEY, key.time, key.val.x, key.val.y, key.val.z, key.val.w);
			}
		}
	}
}

void DumpScaleKeys(Control* cont) 
{
	if (!cont) return;
	
	int i;
	//TSTR indent = GetIndent(indentLevel);
	IKeyControl *ikc = GetKeyControlInterface(cont);
	
	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_SCALE_CLASS_ID, 0))
	{
		int numKeys;
		if (numKeys = ikc->GetNumKeys()) 
		{
			//fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_SCALE_TCB); 
			Write_Log_String(ghLog, "%s\n", ID_CONTROL_SCALE_TCB);

			for (i=0; i<numKeys; i++) 
			{
				ITCBScaleKey key;
				ikc->GetKey(i, &key);
				/*fprintf(pStream, "%s\t\t\t%s %d\t%s",
					indent.data(),
					ID_TCB_SCALE_KEY,
					key.time,
					Format(key.val));*/
				Point3 axis;
				float angle;
				AngAxisFromQ(key.val.q, &angle, axis);
				Write_Log_String(ghLog, "%s время: %d Quat q(axis/angle): %f %f %f %f\n", 
										ID_TCB_SCALE_KEY, key.time, axis.x, axis.y, axis.z, angle);

				//fprintf(pStream, "\t%s\t%s\t%s\t%s\t%s\n", Format(key.tens), Format(key.cont), Format(key.bias), Format(key.easeIn), Format(key.easeOut));*/
				Write_Log_String(ghLog, "ITCBScaleKey key.tens: %f, key.cont: %f key.bias: %f key.easeIn: %f key.easeOut: %f\n", 
									key.tens, key.cont, key.bias, key.easeIn, key.easeOut);
			}
		}
	}
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_SCALE_CLASS_ID, 0)) 
	{
		int numKeys;
		if (numKeys = ikc->GetNumKeys()) 
		{
			//fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_SCALE_BEZIER); 
			Write_Log_String(ghLog, "%s\n", ID_CONTROL_SCALE_BEZIER);

			for (i=0; i<numKeys; i++) 
			{
				IBezScaleKey key;
				ikc->GetKey(i, &key);
				/*fprintf(pStream, "%s\t\t\t%s %d\t%s",
					indent.data(),
					ID_BEZIER_SCALE_KEY,
					key.time,
					Format(key.val));*/
				Point3 axis;
				float angle;
				AngAxisFromQ(key.val.q, &angle, axis);
				Write_Log_String(ghLog, "%s время: %d Quat q(axis/angle): %f %f %f %f scale: %f %f %f\n", 
										ID_BEZIER_SCALE_KEY, key.time, axis.x, axis.y, axis.z, angle,
										key.val.s.x, key.val.s.y, key.val.s.z);

				//fprintf(pStream, "\t%s\t%s\t%d\n", Format(key.intan), Format(key.outtan), key.flags);*/
				Write_Log_String(ghLog, "IBezScaleKey key.intan: %f %f %f, key.outtan: %f %f %f key.flags: %d\n", 
													key.intan.x, key.intan.y, key.intan.z, 
													key.outtan.x, key.outtan.y, key.outtan.z,
													key.flags);
			}
		}
	}
	else if (ikc && cont->ClassID() == Class_ID(LININTERP_SCALE_CLASS_ID, 0)) 
	{
		int numKeys;
		if (numKeys = ikc->GetNumKeys()) 
		{
			//fprintf(pStream,"%s\t\t%s {\n", indent.data(), ID_CONTROL_SCALE_LINEAR); 
			Write_Log_String(ghLog, "%s\n", ID_CONTROL_SCALE_LINEAR);

			for (i=0; i<numKeys; i++) 
			{
				ILinScaleKey key;
				ikc->GetKey(i, &key);
				/*fprintf(pStream, "%s\t\t\t%s %d\t%s\n",
					indent.data(),
					ID_SCALE_KEY,
					key.time,
					Format(key.val));*/
				Point3 axis;
				float angle;
				AngAxisFromQ(key.val.q, &angle, axis);
				Write_Log_String(ghLog, "%s время: %d Quat q(axis/angle): %f %f %f %f\n", 
										ID_SCALE_KEY, key.time, axis.x, axis.y, axis.z, angle);
			}
		}
	}
}

// Not truly the correct way to compare floats of arbitary magnitude...
BOOL EqualPoint3(Point3 p1, Point3 p2)
{
	if (fabs(p1.x - p2.x) > ALMOST_ZERO)
		return FALSE;
	if (fabs(p1.y - p2.y) > ALMOST_ZERO)
		return FALSE;
	if (fabs(p1.z - p2.z) > ALMOST_ZERO)
		return FALSE;

	return TRUE;
}


// Determine if a TM controller is known by the system.
BOOL IsKnownController(Control* cont)
{
	ulong partA, partB;

	if (!cont)
		return FALSE;

	partA = cont->ClassID().PartA();
	partB = cont->ClassID().PartB();

	if (partB != 0x00)
		return FALSE;

	switch (partA) {
		case TCBINTERP_POSITION_CLASS_ID:
		case TCBINTERP_ROTATION_CLASS_ID:
		case TCBINTERP_SCALE_CLASS_ID:
		case HYBRIDINTERP_POSITION_CLASS_ID:
		case HYBRIDINTERP_ROTATION_CLASS_ID:
		case HYBRIDINTERP_SCALE_CLASS_ID:
		case LININTERP_POSITION_CLASS_ID:
		case LININTERP_ROTATION_CLASS_ID:
		case LININTERP_SCALE_CLASS_ID:
			return TRUE;
	}

	return FALSE;
}
