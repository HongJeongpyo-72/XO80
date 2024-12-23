﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Design;
using System.Drawing.Imaging;

namespace GUIDesigner
{
    class CurveWidget : Panel, IWidget
    {
        protected override void OnPaintBackground(PaintEventArgs e)
        {
            if (this.GradientMode == false)
                base.OnPaintBackground(e);
            else
            {
                Rectangle rc = new Rectangle(0, 0, this.ClientSize.Width, this.ClientSize.Height);
                using (LinearGradientBrush brush = new LinearGradientBrush(rc, this.BackColor, this.GradientColor, LinearGradientMode.Vertical))
                {
                    e.Graphics.FillRectangle(brush, rc);
                }
            }
        }

        public CurveWidget()
        {
            SetStyle(ControlStyles.SupportsTransparentBackColor, true);
            base.BackgroundImageLayout = ImageLayout.None;
            this.Compress = true;
            this.External = false;
            this.Dither = true;
            this.PointCount = 0;
            this.Static = false;
        }

        [Editor(typeof(ImageLocationEditor), typeof(UITypeEditor))]
        public override Image BackgroundImage
        {
            get { return base.BackgroundImage; }
            set { base.BackgroundImage = value; }
        }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public virtual ImageLayout BackgroundImageLayout { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public BorderStyle BorderStyle { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public virtual Cursor Cursor { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public virtual Font Font { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public virtual Color ForeColor { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public virtual RightToLeft RightToLeft { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public bool UseWaitCursor { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public virtual bool AllowDrop { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public virtual ContextMenuStrip ContextMenuStrip { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public ImeMode ImeMode { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public int TabIndex { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public bool TabStop { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public string AccessibleDescription { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public string AccessibleName { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public AccessibleRole AccessibleRole { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public virtual AnchorStyles Anchor { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public virtual bool AutoScroll { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public Size AutoScrollMargin { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public Size AutoScrollMinSize { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public override bool AutoSize { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public virtual AutoSizeMode AutoSizeMode { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public virtual DockStyle Dock { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public Padding Margin { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public virtual Size MaximumSize { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public virtual Size MinimumSize { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public Padding Padding { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public bool CausesValidation { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public ControlBindingsCollection DataBindings { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        new public object Tag { get; set; }

        [LocalizedDescription("PixelFormat", typeof(WidgetManual))]
        public ITU.WidgetPixelFormat PixelFormat { get; set; }
        [LocalizedDescription("Compress", typeof(WidgetManual))]
        public Boolean Compress { get; set; }
        [LocalizedDescription("External", typeof(WidgetManual))]
        public Boolean External { get; set; }
        public Boolean Dither { get; set; }

        [LocalizedDescription("GradientColor", typeof(WidgetManual))]
        public Color GradientColor { get; set; }
        [LocalizedDescription("GradientMode", typeof(WidgetManual))]
        public Boolean GradientMode { get; set; }

        public Boolean LineOnly { get; set; }

        [LocalizedDescription("Static", typeof(WidgetManual))]
        public bool Static { get; set; }

        [LocalizedDescription("PointCount", typeof(WidgetManual))]
        public int PointCount { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point00 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point01 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point02 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point03 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point04 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point05 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point06 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point07 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point08 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point09 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point10 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point11 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point12 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point13 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point14 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point15 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point16 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point17 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point18 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point19 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point20 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point21 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point22 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point23 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point24 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point25 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point26 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point27 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point28 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point29 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point30 { get; set; }
        [LocalizedDescription("Point00to31", typeof(WidgetManual))]
        public Point Point31 { get; set; }

        private bool hided = false;
        [LocalizedDescription("Hided", typeof(WidgetManual))]
        public bool Hided
        {
            get
            {
                return hided;
            }

            set
            {
                if (value)
                    Hide();
                else
                    Show();

                hided = value;
            }
        }

        private bool hide_ds = false;
        public bool HideDS
        {
            get
            {
                return hide_ds;
            }
            set
            {
                hide_ds = value;
            }
        }

        public ITUWidget CreateITUWidget()
        {
            PropertyDescriptorCollection properties = TypeDescriptor.GetProperties(this);
            ITUCurve widget = new ITUCurve();
            
            widget.name = this.Name;

            if (!(bool)properties["Enabled"].GetValue(this))
                widget.flags &= ~ITU.ITU_ENABLED;

            widget.visible = (bool)properties["Visible"].GetValue(this);

            widget.active = false;
            widget.dirty = false;
            widget.alpha = 255;
            widget.rect.x = this.Location.X;
            widget.rect.y = this.Location.Y;
            widget.rect.width = this.Size.Width;
            widget.rect.height = this.Size.Height;
            widget.color.alpha = this.BackColor.A;
            widget.color.red = this.BackColor.R;
            widget.color.green = this.BackColor.G;
            widget.color.blue = this.BackColor.B;
            widget.bound.x = 0;
            widget.bound.y = 0;
            widget.bound.width = 0;
            widget.bound.height = 0;

            if (this.BackgroundImage != null)
            {
                widget.staticSurf = ITU.CreateSurfaceNode(this.BackgroundImage as Bitmap, this.PixelFormat, this.Compress, this.External, this.Dither);
            }
            if (this.External)
                widget.flags |= ITU.ITU_EXTERNAL;

            widget.graidentMode = (uint)(this.GradientMode == true ? ITU.WidgetGradientMode.Vertical : ITU.WidgetGradientMode.None);
            widget.gradientColor.alpha = this.GradientColor.A;
            widget.gradientColor.red = this.GradientColor.R;
            widget.gradientColor.green = this.GradientColor.G;
            widget.gradientColor.blue = this.GradientColor.B;

            if (LineOnly)
                widget.curveFlags |= ITUCurve.ITU_CURVE_LINE_ONLY;

            if (Static)
                widget.curveFlags |= ITUCurve.ITU_CURVE_STATIC;

            Point[] points = new Point[]
            {
                this.Point00,
                this.Point01,
                this.Point02,
                this.Point03,
                this.Point04,
                this.Point05,
                this.Point06,
                this.Point07,
                this.Point08,
                this.Point09,
                this.Point10,
                this.Point11,
                this.Point12,
                this.Point13,
                this.Point14,
                this.Point15,
                this.Point16,
                this.Point17,
                this.Point18,
                this.Point19,
                this.Point20,
                this.Point21,
                this.Point22,
                this.Point23,
                this.Point24,
                this.Point25,
                this.Point26,
                this.Point27,
                this.Point28,
                this.Point29,
                this.Point30,
                this.Point31
            };

            widget.pointCount = this.PointCount < ITUCurve.ITU_CURVE_MAX_POINT_COUNT ? this.PointCount : ITUCurve.ITU_CURVE_MAX_POINT_COUNT;
            for (int i = 0; i < widget.pointCount; ++i)
            {
                widget.xPoints[i] = points[i].X;
                widget.yPoints[i] = points[i].Y;
            }
            return widget;
        }

        public void SaveImages(String path)
        {
            if (this.BackgroundImage != null)
            {
                Bitmap bitmap = this.BackgroundImage as Bitmap;
                ITU.SaveImage(bitmap, path , LayerWidget.FindLayerName(this), this.Name + "_BackgroundImage");
            }
        }

        public void WriteFunctions(HashSet<string> functions)
        {
        }

        public bool HasFunctionName(string funcName)
        {
            return false;
        }
    }
}
